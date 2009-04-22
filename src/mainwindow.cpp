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
	status = STATUS_IDLE;
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
	if (status == STATUS_READING)
	{
		if (QMessageBox::warning(NULL, "Exit?", "Exiting now will result in a corrupt image file.\nAre you sure you want to exit?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
			status = STATUS_EXIT;
		event->ignore();
	}
	else if (status == STATUS_WRITING)
	{
		if (QMessageBox::warning(NULL, "Exit?", "Exiting now will result in a corrupt disk.\nAre you sure you want to exit?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
			status = STATUS_EXIT;
		event->ignore();
	}
}

void MainWindow::on_tbBrowse_clicked()
{
	QString filelocation = QFileDialog::getSaveFileName(NULL, "Select a disk image", QString(), "*.img;*.IMG", 0, QFileDialog::DontConfirmOverwrite);
	if (!filelocation.isNull())
		leFile->setText(filelocation);
}

void MainWindow::on_tbRefresh_clicked()
{
	unsigned long availabledrives = GetLogicalDrives();
	int i = 0;
	cboxDevice->clear();
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
}

void MainWindow::on_bCancel_clicked()
{
	if (status == STATUS_READING)
	{
		if (QMessageBox::warning(NULL, "Cancel?", "Canceling now will result in a corrupt image file.\nAre you sure you want to cancel?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
			status = STATUS_IDLE;
	}
	else if (status == STATUS_WRITING)
	{
		if (QMessageBox::warning(NULL, "Cancel?", "Canceling now will result in a corrupt disk.\nAre you sure you want to cancel?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
			status = STATUS_IDLE;
	}
}

void MainWindow::on_bWrite_clicked()
{
	if (!leFile->text().isEmpty())
	{
		QFileInfo fileinfo(leFile->text());
		if (fileinfo.exists() && fileinfo.isFile() && fileinfo.isReadable() && fileinfo.size() > 0)
		{
			if (leFile->text().at(0) == cboxDevice->currentText().at(1))
			{
				QMessageBox::critical(NULL, "Write Error", "Image file cannot be located on the requested device.");
				return;
			}
			if (!spaceAvailable(QString("%1:\\").arg(cboxDevice->currentText().at(1)).toAscii().data(), (unsigned long)fileinfo.size(), false))
			{
				QMessageBox::critical(NULL, "Write Error", "Not enough space on disk.");
				return;
			}
			if (QMessageBox::warning(NULL, "Confirm overwrite", "Writing to a physical device can corrupt the device.\nAre you sure you want to continue?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
				return;
			status = STATUS_WRITING;
			bCancel->setEnabled(true);
			bWrite->setEnabled(false);
			bRead->setEnabled(false);
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
				status = STATUS_IDLE;
				filelocation = NULL;
				bCancel->setEnabled(false);
				bWrite->setEnabled(true);
				bRead->setEnabled(true);
				return;
			}
			if (!getLockOnVolume(hVolume))
			{
				delete filelocation;
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				filelocation = NULL;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				bWrite->setEnabled(true);
				bRead->setEnabled(true);
				return;
			}
			if (!unmountVolume(hVolume))
			{
				delete filelocation;
				removeLockOnVolume(hVolume);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				filelocation = NULL;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				bWrite->setEnabled(true);
				bRead->setEnabled(true);
				return;
			}
			hFile = getHandleOnFile(filelocation, GENERIC_READ);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				delete filelocation;
				removeLockOnVolume(hVolume);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				filelocation = NULL;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				bWrite->setEnabled(true);
				bRead->setEnabled(true);
				return;
			}
			hRawDisk = getHandleOnDevice(deviceID, GENERIC_WRITE);
			if (hRawDisk == INVALID_HANDLE_VALUE)
			{
				delete filelocation;
				removeLockOnVolume(hVolume);
				CloseHandle(hFile);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				filelocation = NULL;
				hVolume = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				bWrite->setEnabled(true);
				bRead->setEnabled(true);
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
			for (i = 0ul; i < numsectors && status == STATUS_WRITING; i += 1024ul)
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
					status = STATUS_IDLE;
					filelocation = NULL;
					sectorData = NULL;
					hRawDisk = INVALID_HANDLE_VALUE;
					hFile = INVALID_HANDLE_VALUE;
					hVolume = INVALID_HANDLE_VALUE;
					bCancel->setEnabled(false);
					bWrite->setEnabled(true);
					bRead->setEnabled(true);
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
					status = STATUS_IDLE;
					filelocation = NULL;
					sectorData = NULL;
					hRawDisk = INVALID_HANDLE_VALUE;
					hFile = INVALID_HANDLE_VALUE;
					hVolume = INVALID_HANDLE_VALUE;
					bCancel->setEnabled(false);
					bWrite->setEnabled(true);
					bRead->setEnabled(true);
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
		else if (!fileinfo.exists() || !fileinfo.isFile())
			QMessageBox::critical(NULL, "File Error", "The selected file does not exist.");
		else if (!fileinfo.isReadable())
			QMessageBox::critical(NULL, "File Error", "You do not have permision to read the selected file.");
		else if (fileinfo.size() == 0)
			QMessageBox::critical(NULL, "File Error", "The specified file contains no data.");
		progressbar->reset();
	  statusbar->showMessage("Done.");
		bCancel->setEnabled(false);
		bWrite->setEnabled(true);
		bRead->setEnabled(true);
	}
	else
		QMessageBox::critical(NULL, "File Error", "Please specify an image file to use.");
	if (status == STATUS_EXIT)
		close();
	status = STATUS_IDLE;
}

void MainWindow::on_bRead_clicked()
{
	if (!leFile->text().isEmpty())
	{
		QFileInfo fileinfo(leFile->text());
		if (leFile->text().at(0) == cboxDevice->currentText().at(1))
		{
			QMessageBox::critical(NULL, "Write Error", "Image file cannot be located on the requested device.");
			return;
		}
		if (fileinfo.exists())
			if (QMessageBox::warning(NULL, "Confirm Overwrite", "Are you sure you want to overwrite the specified file?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::No)
				return;
		bCancel->setEnabled(true);
		bWrite->setEnabled(false);
		bRead->setEnabled(false);
		status = STATUS_READING;
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
			status = STATUS_IDLE;
			filelocation = NULL;
			bCancel->setEnabled(false);
			bWrite->setEnabled(true);
			bRead->setEnabled(true);
			return;
		}
		if (!getLockOnVolume(hVolume))
		{
			delete filelocation;
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			filelocation = NULL;
			hVolume = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			bWrite->setEnabled(true);
			bRead->setEnabled(true);
			return;
		}
		if (!unmountVolume(hVolume))
		{
			delete filelocation;
			removeLockOnVolume(hVolume);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			filelocation = NULL;
			hVolume = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			bWrite->setEnabled(true);
			bRead->setEnabled(true);
			return;
		}
		hFile = getHandleOnFile(filelocation, GENERIC_WRITE);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			delete filelocation;
			removeLockOnVolume(hVolume);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			filelocation = NULL;
			hVolume = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			bWrite->setEnabled(true);
			bRead->setEnabled(true);
			return;
		}
		hRawDisk = getHandleOnDevice(deviceID, GENERIC_READ);
		if (hRawDisk == INVALID_HANDLE_VALUE)
		{
			delete filelocation;
			removeLockOnVolume(hVolume);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			filelocation = NULL;
			hVolume = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			bWrite->setEnabled(true);
			bRead->setEnabled(true);
			return;
		}
		numsectors = getNumberOfSectors(hRawDisk, &sectorsize);
		if (!spaceAvailable(leFile->text().left(3).toAscii().data(), numsectors * sectorsize, true))
		{
			QMessageBox::critical(NULL, "Write Error", "Disk is not large enough for the specified image.");
			delete filelocation;
			removeLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			filelocation = NULL;
			sectorData = NULL;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			bWrite->setEnabled(true);
			bRead->setEnabled(true);
			return;
		}
		if (numsectors == 0ul)
			progressbar->setRange(0, 100);
		else
			progressbar->setRange(0, (int)numsectors);
		lasti = 0ul;
		timer.start();
		for (i = 0ul; i < numsectors && status == STATUS_READING; i += 1024ul)
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
				status = STATUS_IDLE;
				filelocation = NULL;
				sectorData = NULL;
				hRawDisk = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				bWrite->setEnabled(true);
				bRead->setEnabled(true);
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
				status = STATUS_IDLE;
				filelocation = NULL;
				sectorData = NULL;
				hRawDisk = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				bWrite->setEnabled(true);
				bRead->setEnabled(true);
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
		bCancel->setEnabled(false);
		bWrite->setEnabled(true);
		bRead->setEnabled(true);
	}
	else
		QMessageBox::critical(NULL, "File Info", "Please specify a file to save data to.");
	if (status == STATUS_EXIT)
		close();
	status = STATUS_IDLE;
}
