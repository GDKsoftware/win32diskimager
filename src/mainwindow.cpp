/**********************************************************************
 *  This program is free software; you can redistribute it and/or     *
 *  modify it under the terms of the GNU General Public License       *
 *  as published by the Free Software Foundation; either version 2    *
 *  of the License, or (at your option) any later version.            *
 *                                                                    *
 *  This program is distributed in the hope that it will be useful,   *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     *
 *  GNU General Public License for more details.                      *
 *                                                                    *
 *  You should have received a copy of the GNU General Public License *
 *  along with this program; if not, write to the Free Software       *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,                *
 *  Boston, MA  02110-1301, USA.                                      *
 *                                                                    *
 *  ---                                                               *
 *  Copyright (C) 2009, Justin Davis <tuxdavis@gmail.com>             *
 **********************************************************************/

#include <QtGui>
#include <QCoreApplication>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winioctl.h>
#include "disk.h"
#include "mainwindow.h"

extern QApplication *app;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);
	unsigned long availabledrives = GetLogicalDrives();
	int i = 0;
	while (availabledrives != 0)
	{
		if ((availabledrives % 2) != 0)
		{
			char drivename[] = "\\\\.\\A:\\";
			drivename[4] += i;
			if (DRIVE_REMOVABLE == GetDriveType(drivename))
			{
				cboxDevice->addItem(QString("[%1]").arg(drivename[4]), QVariant(getPhysicalDeviceID(i)));
			}
		}
		availabledrives >>= 1;
		cboxDevice->setCurrentIndex(0);
		++i;
	}
	noclose = false;
	progressbar->reset();
	statusbar->showMessage("Waiting for a task.");
	hVolume = INVALID_HANDLE_VALUE;
	hFile = INVALID_HANDLE_VALUE;
	hRawDisk = INVALID_HANDLE_VALUE;
	filelocation = NULL;
	sectorData = NULL;
	sectorsize = 0ul;
}

MainWindow::~MainWindow()
{
	if (hRawDisk != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hRawDisk);
		hRawDisk = INVALID_HANDLE_VALUE;
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	if (hVolume != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hVolume);
		hVolume = INVALID_HANDLE_VALUE;
	}
	if (filelocation != NULL)
	{
		delete filelocation;
		filelocation = NULL;
	}
	if (sectorData != NULL)
	{
		delete sectorData;
		sectorData = NULL;
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (noclose)
     event->ignore();
  else
      event->accept();
}

void MainWindow::on_tbBrowse_clicked()
{
	QString filelocation = QFileDialog::getSaveFileName(NULL, "Select a disk image", QString(), "*.img;*.IMG", 0, QFileDialog::DontConfirmOverwrite);
	if (!filelocation.isNull())
		leFile->setText(filelocation);
}

void MainWindow::on_bBurn_clicked()
{
		noclose = true;
	QFileInfo fileinfo(leFile->text());
	if (fileinfo.exists())
	{
		double mbpersec;
		unsigned long i, lasti, numsectors;
		int volumeID = cboxDevice->currentText().at(1).toAscii() - 'A';
		int deviceID = cboxDevice->itemData(cboxDevice->currentIndex()).toInt();
		filelocation = new char[5 + leFile->text().length()];
		sprintf(filelocation, "\\\\.\\%s", leFile->text().toAscii().data());
		hVolume = getHandleOnVolume(volumeID, GENERIC_WRITE);
		if (hVolume == INVALID_HANDLE_VALUE)
		{
			delete filelocation;
			noclose = false;
			filelocation = NULL;
			return;
		}
		if (!getLockOnVolume(hVolume))
		{
			delete filelocation;
			CloseHandle(hVolume);
			noclose = false;
			filelocation = NULL;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		if (!unmountVolume(hVolume))
		{
			delete filelocation;
			removeLockOnVolume(hVolume);
			CloseHandle(hVolume);
			noclose = false;
			filelocation = NULL;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		hFile = getHandleOnFile(filelocation, GENERIC_READ);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			delete filelocation;
			removeLockOnVolume(hVolume);
			CloseHandle(hVolume);
			noclose = false;
			filelocation = NULL;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		hRawDisk = getHandleOnDevice(deviceID, GENERIC_WRITE);
		if (hRawDisk == INVALID_HANDLE_VALUE)
		{
			delete filelocation;
			removeLockOnVolume(hVolume);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			noclose = false;
			filelocation = NULL;
			hVolume = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			return;
		}
		getNumberOfSectors(hRawDisk, &sectorsize);
		numsectors = getFileSizeInSectors(hFile, sectorsize);
		if (numsectors == 0ul)
			progressbar->setRange(0, 100);
		else
			progressbar->setRange(0, (int)numsectors);
		lasti = 0ul;
		timer.start();
		for (i = 0ul; i < numsectors; i += 1024ul)
		{
			sectorData = readSectorDataFromHandle(hFile, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), sectorsize);
			if (sectorData == NULL)
			{
				delete filelocation;
				delete sectorData;
				removeLockOnVolume(hVolume);
				CloseHandle(hRawDisk);
				CloseHandle(hFile);
				CloseHandle(hVolume);
				noclose = false;
				filelocation = NULL;
				sectorData = NULL;
				hRawDisk = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				hVolume = INVALID_HANDLE_VALUE;
				return;
			}
			if (!writeSectorDataToHandle(hRawDisk, sectorData, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), sectorsize))
			{
				delete filelocation;
				delete sectorData;
				removeLockOnVolume(hVolume);
				CloseHandle(hRawDisk);
				CloseHandle(hFile);
				CloseHandle(hVolume);
				noclose = false;
				filelocation = NULL;
				sectorData = NULL;
				hRawDisk = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				hVolume = INVALID_HANDLE_VALUE;
				return;
			}
			delete sectorData;
			sectorData = NULL;
			QCoreApplication::processEvents();
			if (timer.elapsed() >= 1000)
			{
				mbpersec = (((double)sectorsize * (i - lasti)) * (1000.0 / timer.elapsed())) / 1024.0 / 1024.0;
				statusbar->showMessage(QString("%1Mb/s").arg(mbpersec));
				timer.start();
				lasti = i;
			}
			progressbar->setValue(i);
			QCoreApplication::processEvents();
		}
		delete filelocation;
		removeLockOnVolume(hVolume);
		CloseHandle(hRawDisk);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		filelocation = NULL;
		sectorData = NULL;
		hRawDisk = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		hVolume = INVALID_HANDLE_VALUE;
	}
	else
		QMessageBox::warning(NULL, "File Error", "The selected file does not exist.");
	progressbar->reset();
  noclose = false;
  statusbar->showMessage("Done.");
}

void MainWindow::on_bRip_clicked()
{
	noclose = true;
	double mbpersec;
	unsigned long i, lasti, numsectors;
	int volumeID = cboxDevice->currentText().at(1).toAscii() - 'A';
	int deviceID = cboxDevice->itemData(cboxDevice->currentIndex()).toInt();
	filelocation = new char[5 + leFile->text().length()];
	sprintf(filelocation, "\\\\.\\%s", leFile->text().toAscii().data());
	hVolume = getHandleOnVolume(volumeID, GENERIC_READ);
	if (hVolume == INVALID_HANDLE_VALUE)
	{
		delete filelocation;
		noclose = false;
		filelocation = NULL;
		return;
	}
	if (!getLockOnVolume(hVolume))
	{
		delete filelocation;
		CloseHandle(hVolume);
		noclose = false;
		filelocation = NULL;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	if (!unmountVolume(hVolume))
	{
		delete filelocation;
		removeLockOnVolume(hVolume);
		CloseHandle(hVolume);
		noclose = false;
		filelocation = NULL;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	hFile = getHandleOnFile(filelocation, GENERIC_WRITE);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		delete filelocation;
		removeLockOnVolume(hVolume);
		CloseHandle(hVolume);
		noclose = false;
		filelocation = NULL;
		hVolume = INVALID_HANDLE_VALUE;
		return;
	}
	hRawDisk = getHandleOnDevice(deviceID, GENERIC_READ);
	if (hRawDisk == INVALID_HANDLE_VALUE)
	{
		delete filelocation;
		removeLockOnVolume(hVolume);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		noclose = false;
		filelocation = NULL;
		hVolume = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		return;
	}
	numsectors = getNumberOfSectors(hRawDisk, &sectorsize);
	if (numsectors == 0ul)
		progressbar->setRange(0, 100);
	else
		progressbar->setRange(0, (int)numsectors);
	lasti = 0ul;
	timer.start();
	for (i = 0ul; i < numsectors; i += 1024ul)
	{
		sectorData = readSectorDataFromHandle(hRawDisk, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), sectorsize);
		if (sectorData == NULL)
		{
			delete filelocation;
			delete sectorData;
			removeLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			noclose = false;
			filelocation = NULL;
			sectorData = NULL;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		if (!writeSectorDataToHandle(hFile, sectorData, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), sectorsize))
		{
			delete filelocation;
			delete sectorData;
			removeLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			noclose = false;
			filelocation = NULL;
			sectorData = NULL;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			return;
		}
		delete sectorData;
		sectorData = NULL;
		if (timer.elapsed() >= 1000)
		{
			mbpersec = (((double)sectorsize * (i - lasti)) * (1000.0 / timer.elapsed())) / 1024.0 / 1024.0;
			statusbar->showMessage(QString("%1Mb/s").arg(mbpersec));
			timer.start();
			lasti = i;
		}
		progressbar->setValue(i);
		QCoreApplication::processEvents();
	}
	delete filelocation;
	removeLockOnVolume(hVolume);
	CloseHandle(hRawDisk);
	CloseHandle(hFile);
	CloseHandle(hVolume);
	filelocation = NULL;
	sectorData = NULL;
	hRawDisk = INVALID_HANDLE_VALUE;
	hFile = INVALID_HANDLE_VALUE;
	hVolume = INVALID_HANDLE_VALUE;
	progressbar->reset();
  statusbar->showMessage("Done.");
	noclose = false;
}
