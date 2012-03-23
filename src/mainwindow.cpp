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
 *  Copyright (C) 2009, 2012 ImageWriter developers                   *
 *                           https://launchpad.net/~image-writer-devs *
 **********************************************************************/

#ifndef WINVER
#define WINVER 0x0500
#endif

#include <QtGui>
#include <QCoreApplication>
#include <QFileInfo>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <winioctl.h>
#include <dbt.h>
#include "disk.h"
#include "mainwindow.h"
#include "md5.h"

extern QApplication *app;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);
	getLogicalDrives();
	status = STATUS_IDLE;
	progressbar->reset();
	statusbar->showMessage("Waiting for a task.");
	hVolume = INVALID_HANDLE_VALUE;
	hFile = INVALID_HANDLE_VALUE;
	hRawDisk = INVALID_HANDLE_VALUE;
	if (QCoreApplication::arguments().count() > 1)
	{
		QString filelocation = QApplication::arguments().at(1);
		QFileInfo fileInfo(filelocation);
		leFile->setText(fileInfo.absoluteFilePath());
	}

	setReadWriteButtonState();

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
	if (sectorData != NULL)
	{
		delete sectorData;
		sectorData = NULL;
	}
}

void MainWindow::setReadWriteButtonState()
{
	bool fileSelected = !(leFile->text().isEmpty());
	bool deviceSelected = (cboxDevice->count() > 0);

	// set read and write buttons according to status of file/device
	bRead->setEnabled(deviceSelected && fileSelected);
	bWrite->setEnabled(deviceSelected && fileSelected);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (status == STATUS_READING)
	{
		if (QMessageBox::warning(NULL, "Exit?", "Exiting now will result in a corrupt image file.\nAre you sure you want to exit?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
		{
			status = STATUS_EXIT;
		}
		event->ignore();
	}
	else if (status == STATUS_WRITING)
	{
		if (QMessageBox::warning(NULL, "Exit?", "Exiting now will result in a corrupt disk.\nAre you sure you want to exit?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
		{
			status = STATUS_EXIT;
		}
		event->ignore();
	}
}

void MainWindow::on_tbBrowse_clicked()
{
	QString filelocation = QFileDialog::getSaveFileName(NULL, "Select a disk image", QString(), "*.img;*.IMG;;*.*", 0, QFileDialog::DontConfirmOverwrite);
	if (!filelocation.isNull())
	{
		leFile->setText(filelocation);
		md5label->clear();

		// if the md5 checkbox is checked, verify that it's a good file
		// and then generate the md5 hash
		if(md5CheckBox->isChecked())
		{
			QFileInfo fileInfo(filelocation);

			if (fileInfo.exists() && fileInfo.isFile() &&
				fileInfo.isReadable() && (fileInfo.size() > 0) )
			{
				generateMd5(filelocation.toLatin1().data());
			}
		}
	}
}

// generates the md5 hash
void MainWindow::generateMd5(char *filename)
{
	md5label->setText("Generating...");
	QApplication::processEvents();

	MD5 md5;

	// may take a few secs - display a wait cursor
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	// "digestFile" computes the md5 - display it in the textbox
	md5label->setText(md5.digestFile(filename));

	// redisplay the normal cursor
	QApplication::restoreOverrideCursor();
}

void MainWindow::on_leFile_textChanged(const QString &qs)
{
	setReadWriteButtonState();

	// if the box was cleared, clear any existing md5 hash
	if( leFile->text().isEmpty() )
	{
		md5label->clear();
	}
}

// on an "editingFinished" signal (IE: return press), if the lineedit
// contains a valid file, and generate the md5
void MainWindow::on_leFile_editingFinished()
{
	if(md5CheckBox->isChecked())
	{
		QFileInfo fileinfo(leFile->text());
		if (fileinfo.exists() && fileinfo.isFile() &&
			fileinfo.isReadable() && (fileinfo.size() > 0) )
		{
			generateMd5(leFile->text().toLatin1().data());
		}
	}
}

void MainWindow::on_bCancel_clicked()
{
	if ( (status == STATUS_READING) || (status == STATUS_WRITING) )

	{
		if (QMessageBox::warning(NULL, "Cancel?", "Canceling now will result in a corrupt destination.\nAre you sure you want to cancel?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
		{
			status = STATUS_IDLE;
		}
	}
}

// if the md5 checkbox becomes "checked", verify the file and generate md5
// when it's "unchecked", clear the md5 label
void MainWindow::on_md5CheckBox_stateChanged()
{
	bool state = md5CheckBox->isChecked();

	md5header->setEnabled(state);
	md5label->setEnabled(state);

	if(state)
	{
		// changed from unchecked to checked
		if( !(leFile->text().isEmpty()) )
		{
			QFileInfo fileinfo(leFile->text());
			if (fileinfo.exists() && fileinfo.isFile() &&
				fileinfo.isReadable() && (fileinfo.size() > 0) )
			{
				generateMd5(leFile->text().toLatin1().data());
			}
		}
		
	}
	else
	{
		// changed from checked to unchecked
		md5label->clear();
	}
}

void MainWindow::on_bWrite_clicked()
{
	if (!leFile->text().isEmpty())
	{
		QFileInfo fileinfo(leFile->text());
		if (fileinfo.exists() && fileinfo.isFile() &&
			fileinfo.isReadable() && (fileinfo.size() > 0) )
		{
			if (leFile->text().at(0) == cboxDevice->currentText().at(1))
			{
				QMessageBox::critical(NULL, "Write Error", "Image file cannot be located on the requested device.");
				return;
			}
			if (QMessageBox::warning(NULL, "Confirm overwrite", "Writing to a physical device can corrupt the device.\nAre you sure you want to continue?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
			{
				return;
			}
			status = STATUS_WRITING;
			bCancel->setEnabled(true);
			bWrite->setEnabled(false);
			bRead->setEnabled(false);
			double mbpersec;
			unsigned long long i, lasti, availablesectors, numsectors;
			int volumeID = cboxDevice->currentText().at(1).toAscii() - 'A';
			int deviceID = cboxDevice->itemData(cboxDevice->currentIndex()).toInt();
			hVolume = getHandleOnVolume(volumeID, GENERIC_WRITE);
			if (hVolume == INVALID_HANDLE_VALUE)
			{
				status = STATUS_IDLE;
				bCancel->setEnabled(false);
				setReadWriteButtonState();
				return;
			}
			if (!getLockOnVolume(hVolume))
			{
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				setReadWriteButtonState();
				return;
			}
			if (!unmountVolume(hVolume))
			{
				removeLockOnVolume(hVolume);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				setReadWriteButtonState();
				return;
			}
			hFile = getHandleOnFile(leFile->text().toAscii().data(), GENERIC_READ);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				removeLockOnVolume(hVolume);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				setReadWriteButtonState();
				return;
			}
			hRawDisk = getHandleOnDevice(deviceID, GENERIC_WRITE);
			if (hRawDisk == INVALID_HANDLE_VALUE)
			{
				removeLockOnVolume(hVolume);
				CloseHandle(hFile);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				hVolume = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				setReadWriteButtonState();
				return;
			}
			availablesectors = getNumberOfSectors(hRawDisk, &sectorsize);
			numsectors = getFileSizeInSectors(hFile, sectorsize);
			if (numsectors > availablesectors)
			{
				QMessageBox::critical(NULL, "Write Error", QString("Not enough space on disk: Size: %1 sectors  Available: %2 sectors  Sector size: %3").arg(numsectors).arg(availablesectors).arg(sectorsize));
				removeLockOnVolume(hVolume);
				CloseHandle(hRawDisk);
				CloseHandle(hFile);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				hVolume = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				hRawDisk = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				setReadWriteButtonState();
				return;
			}

			progressbar->setRange(0, (numsectors == 0ul) ? 100 : (int)numsectors);
			lasti = 0ul;
			timer.start();
			for (i = 0ul; i < numsectors && status == STATUS_WRITING; i += 1024ul)
			{
				sectorData = readSectorDataFromHandle(hFile, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), sectorsize);
				if (sectorData == NULL)
				{
					delete sectorData;
					removeLockOnVolume(hVolume);
					CloseHandle(hRawDisk);
					CloseHandle(hFile);
					CloseHandle(hVolume);
					status = STATUS_IDLE;
					sectorData = NULL;
					hRawDisk = INVALID_HANDLE_VALUE;
					hFile = INVALID_HANDLE_VALUE;
					hVolume = INVALID_HANDLE_VALUE;
					bCancel->setEnabled(false);
					setReadWriteButtonState();
					return;
				}
				if (!writeSectorDataToHandle(hRawDisk, sectorData, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), sectorsize))
				{
					delete sectorData;
					removeLockOnVolume(hVolume);
					CloseHandle(hRawDisk);
					CloseHandle(hFile);
					CloseHandle(hVolume);
					status = STATUS_IDLE;
					sectorData = NULL;
					hRawDisk = INVALID_HANDLE_VALUE;
					hFile = INVALID_HANDLE_VALUE;
					hVolume = INVALID_HANDLE_VALUE;
					bCancel->setEnabled(false);
					setReadWriteButtonState();
					return;
				}
				delete sectorData;
				sectorData = NULL;
				QCoreApplication::processEvents();
				if (timer.elapsed() >= 1000)
				{
					mbpersec = (((double)sectorsize * (i - lasti)) * (1000.0 / timer.elapsed())) / 1024.0 / 1024.0;
                                        statusbar->showMessage(QString("%1MB/s").arg(mbpersec));
					timer.start();
					lasti = i;
				}
				progressbar->setValue(i);
				QCoreApplication::processEvents();
			}
			removeLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			sectorData = NULL;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
		}
		else if (!fileinfo.exists() || !fileinfo.isFile())
		{
			QMessageBox::critical(NULL, "File Error", "The selected file does not exist.");
		}
		else if (!fileinfo.isReadable())
		{
			QMessageBox::critical(NULL, "File Error", "You do not have permision to read the selected file.");
		}
		else if (fileinfo.size() == 0)
		{
			QMessageBox::critical(NULL, "File Error", "The specified file contains no data.");
		}
		progressbar->reset();
		statusbar->showMessage("Done.");
		bCancel->setEnabled(false);
		setReadWriteButtonState();
		QMessageBox::information(NULL, "Complete", "Write Successful.");
	}
	else
	{
		QMessageBox::critical(NULL, "File Error", "Please specify an image file to use.");
	}
	if (status == STATUS_EXIT)
	{
		close();
	}
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
		{
			if (QMessageBox::warning(NULL, "Confirm Overwrite", "Are you sure you want to overwrite the specified file?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::No)
			{
				return;
			}
		}
		bCancel->setEnabled(true);
		bWrite->setEnabled(false);
		bRead->setEnabled(false);
		status = STATUS_READING;
		double mbpersec;
		unsigned long long i, lasti, numsectors, filesize, spaceneeded = 0ull;
		int volumeID = cboxDevice->currentText().at(1).toAscii() - 'A';
		int deviceID = cboxDevice->itemData(cboxDevice->currentIndex()).toInt();
		hVolume = getHandleOnVolume(volumeID, GENERIC_READ);
		if (hVolume == INVALID_HANDLE_VALUE)
		{
			status = STATUS_IDLE;
			bCancel->setEnabled(false);
			setReadWriteButtonState();
			return;
		}
		if (!getLockOnVolume(hVolume))
		{
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			setReadWriteButtonState();
			return;
		}
		if (!unmountVolume(hVolume))
		{
			removeLockOnVolume(hVolume);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			setReadWriteButtonState();
			return;
		}
		hFile = getHandleOnFile(leFile->text().toAscii().data(), GENERIC_WRITE);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			removeLockOnVolume(hVolume);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			setReadWriteButtonState();
			return;
		}
		hRawDisk = getHandleOnDevice(deviceID, GENERIC_READ);
		if (hRawDisk == INVALID_HANDLE_VALUE)
		{
			removeLockOnVolume(hVolume);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			hVolume = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			setReadWriteButtonState();
			return;
		}
		numsectors = getNumberOfSectors(hRawDisk, &sectorsize);
		filesize = getFileSizeInSectors(hFile, sectorsize);
		if (filesize >= numsectors)
		{
			spaceneeded = 0ull;
		}
		else
		{
			spaceneeded = (unsigned long long)(numsectors - filesize) * (unsigned long long)(sectorsize);
		}
		if (!spaceAvailable(leFile->text().left(3).replace(QChar('/'), QChar('\\')).toAscii().data(), spaceneeded))
		{
			QMessageBox::critical(NULL, "Write Error", "Disk is not large enough for the specified image.");
			removeLockOnVolume(hVolume);
			CloseHandle(hRawDisk);
			CloseHandle(hFile);
			CloseHandle(hVolume);
			status = STATUS_IDLE;
			sectorData = NULL;
			hRawDisk = INVALID_HANDLE_VALUE;
			hFile = INVALID_HANDLE_VALUE;
			hVolume = INVALID_HANDLE_VALUE;
			bCancel->setEnabled(false);
			setReadWriteButtonState();
			return;
		}
		if (numsectors == 0ul)
		{
			progressbar->setRange(0, 100);
		}
		else
		{
			progressbar->setRange(0, (int)numsectors);
		}
		lasti = 0ul;
		timer.start();
		for (i = 0ul; i < numsectors && status == STATUS_READING; i += 1024ul)
		{
			sectorData = readSectorDataFromHandle(hRawDisk, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), sectorsize);
			if (sectorData == NULL)
			{
				delete sectorData;
				removeLockOnVolume(hVolume);
				CloseHandle(hRawDisk);
				CloseHandle(hFile);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				sectorData = NULL;
				hRawDisk = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				setReadWriteButtonState();
				return;
			}
			if (!writeSectorDataToHandle(hFile, sectorData, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), sectorsize))
			{
				delete sectorData;
				removeLockOnVolume(hVolume);
				CloseHandle(hRawDisk);
				CloseHandle(hFile);
				CloseHandle(hVolume);
				status = STATUS_IDLE;
				sectorData = NULL;
				hRawDisk = INVALID_HANDLE_VALUE;
				hFile = INVALID_HANDLE_VALUE;
				hVolume = INVALID_HANDLE_VALUE;
				bCancel->setEnabled(false);
				setReadWriteButtonState();
				return;
			}
			delete sectorData;
			sectorData = NULL;
			if (timer.elapsed() >= 1000)
			{
				mbpersec = (((double)sectorsize * (i - lasti)) * (1000.0 / timer.elapsed())) / 1024.0 / 1024.0;
				statusbar->showMessage(QString("%1MB/s").arg(mbpersec));
				timer.start();
				lasti = i;
			}
			progressbar->setValue(i);
			QCoreApplication::processEvents();
		}
		removeLockOnVolume(hVolume);
		CloseHandle(hRawDisk);
		CloseHandle(hFile);
		CloseHandle(hVolume);
		sectorData = NULL;
		hRawDisk = INVALID_HANDLE_VALUE;
		hFile = INVALID_HANDLE_VALUE;
		hVolume = INVALID_HANDLE_VALUE;
		progressbar->reset();
		statusbar->showMessage("Done.");
		bCancel->setEnabled(false);
		setReadWriteButtonState();
		QMessageBox::information(NULL, "Complete", "Read Successful.");
	}
	else
	{
		QMessageBox::critical(NULL, "File Info", "Please specify a file to save data to.");
	}
	if (status == STATUS_EXIT)
	{
		close();
	}
	status = STATUS_IDLE;
}

// getLogicalDrives sets cBoxDevice with any logical drives found, as long
// as they indicate that they're either removable, or fixed and on USB bus
void MainWindow::getLogicalDrives()
{
	// GetLogicalDrives returns 0 on failure, or a bitmask representing
	// the drives available on the system (bit 0 = A:, bit 1 = B:, etc)
	unsigned long driveMask = GetLogicalDrives();
	int i = 0;
	ULONG pID;

	cboxDevice->clear();

	while (driveMask != 0)
	{
		if (driveMask & 1)
		{
			// the "A" in drivename will get incremented by the # of bits
			// we've shifted
			char drivename[] = "\\\\.\\A:\\";
			drivename[4] += i;
			if (checkDriveType(drivename, &pID))
			{
				cboxDevice->addItem(QString("[%1:\\]").arg(drivename[4]), (qulonglong)pID);
			}
		}
		driveMask >>= 1;
		cboxDevice->setCurrentIndex(0);
		++i;
	}
}

// support routine for winEvent - returns the drive letter for a given mask
//   taken from http://support.microsoft.com/kb/163503
char FirstDriveFromMask (ULONG unitmask)
{
	char i;

	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)
		{
			break;
		}
		unitmask = unitmask >> 1;
	}

	return (i + 'A');
}

// register to receive notifications when USB devices are inserted or removed
// adapted from http://www.known-issues.net/qt/qt-detect-event-windows.html
bool MainWindow::winEvent ( MSG * msg, long * result )
{
	if(msg->message == WM_DEVICECHANGE)
	{
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
		switch(msg->wParam)
		{
			case DBT_DEVICEARRIVAL:
				if (lpdb -> dbch_devicetype == DBT_DEVTYP_VOLUME)
				{
					PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
					if(DBTF_NET)
					{
						char ALET = FirstDriveFromMask(lpdbv->dbcv_unitmask);
						// add device to combo box (after sanity check that
						// it's not already there, which it shouldn't be)
						QString qs = QString("[%1:\\]").arg(ALET);
						if (cboxDevice->findText(qs) == -1)
						{
							ULONG pID;
							char longname[] = "\\\\.\\A:\\";
							longname[4] = ALET;
							// checkDriveType gets the physicalID
							if (checkDriveType(longname, &pID))
							{
								cboxDevice->addItem(qs, (qulonglong)pID);
								setReadWriteButtonState();
							}
						}
					}
				}
				break;
			case DBT_DEVICEREMOVECOMPLETE:
				if (lpdb -> dbch_devicetype == DBT_DEVTYP_VOLUME)
				{
					PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
					if(DBTF_NET)
					{
						char ALET = FirstDriveFromMask(lpdbv->dbcv_unitmask);
						//  find the device that was removed in the combo box,
						//  and remove it from there....
						//  "removeItem" ignores the request if the index is
						//  out of range, and findText returns -1 if the item isn't found.
						cboxDevice->removeItem(cboxDevice->findText(QString("[%1:\\]").arg(ALET)));
						setReadWriteButtonState();
					}
				}
				break;
		} // skip the rest
	} // end of if msg->message
	return false; // let qt handle the rest
}
