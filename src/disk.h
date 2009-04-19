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

#ifndef DISK_H
#define DISK_H

#include <QtGui>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winioctl.h>
#ifndef FSCTL_IS_VOLUME_MOUNTED
#define FSCTL_IS_VOLUME_MOUNTED         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif // FSCTL_IS_VOLUME_MOUNTED

typedef struct DEVICE_NUMBER {
  DEVICE_TYPE  DeviceType;
  ULONG  DeviceNumber;
  ULONG  PartitionNumber;
} DEVICE_NUMBER, *PDEVICE_NUMBER;

int getPhysicalDeviceID(int device);
HANDLE getHandleOnFile(char *filelocation, DWORD access);
HANDLE getHandleOnDevice(int device, DWORD access);
HANDLE getHandleOnVolume(int volume, DWORD access);
bool getLockOnVolume(HANDLE handle);
bool removeLockOnVolume(HANDLE handle);
bool unmountVolume(HANDLE handle);
bool isVolumeUnmounted(HANDLE handle);
char *readSectorDataFromHandle(HANDLE handle, unsigned long startsector, unsigned long numsectors, unsigned long sectorsize);
bool writeSectorDataToHandle(HANDLE handle, char *data, unsigned long startsector, unsigned long numsectors, unsigned long sectorsize);
unsigned long getNumberOfSectors(HANDLE handle, unsigned long *sectorsize);
unsigned long getFileSizeInSectors(HANDLE handle, unsigned long sectorsize);

#endif // DISK_H
