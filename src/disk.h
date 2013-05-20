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

#ifndef DISK_H
#define DISK_H

#ifndef WINVER
#define WINVER 0x0500
#endif

#include <QtGui>
#include <QString>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <winioctl.h>
#ifndef FSCTL_IS_VOLUME_MOUNTED
#define FSCTL_IS_VOLUME_MOUNTED         CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif // FSCTL_IS_VOLUME_MOUNTED

typedef struct _DEVICE_NUMBER
{
    DEVICE_TYPE  DeviceType;
    ULONG  DeviceNumber;
    ULONG  PartitionNumber;
} DEVICE_NUMBER, *PDEVICE_NUMBER;

// IOCTL control code
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

// retrieve the storage device descriptor data for a device.
typedef struct _STORAGE_DEVICE_DESCRIPTOR
{
    ULONG  Version;
    ULONG  Size;
    UCHAR  DeviceType;
    UCHAR  DeviceTypeModifier;
    BOOLEAN  RemovableMedia;
    BOOLEAN  CommandQueueing;
    ULONG  VendorIdOffset;
    ULONG  ProductIdOffset;
    ULONG  ProductRevisionOffset;
    ULONG  SerialNumberOffset;
    STORAGE_BUS_TYPE  BusType;
    ULONG  RawPropertiesLength;
    UCHAR  RawDeviceProperties[1];
} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

// retrieve the properties of a storage device or adapter.
typedef enum _STORAGE_QUERY_TYPE
{
    PropertyStandardQuery = 0,
    PropertyExistsQuery,
    PropertyMaskQuery,
    PropertyQueryMaxDefined
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

// retrieve the properties of a storage device or adapter.
typedef enum _STORAGE_PROPERTY_ID
{
    StorageDeviceProperty = 0,
    StorageAdapterProperty,
    StorageDeviceIdProperty
} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

// retrieve the properties of a storage device or adapter.
typedef struct _STORAGE_PROPERTY_QUERY
{
    STORAGE_PROPERTY_ID  PropertyId;
    STORAGE_QUERY_TYPE  QueryType;
    UCHAR  AdditionalParameters[1];
} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

HANDLE getHandleOnFile(char *filelocation, DWORD access);
HANDLE getHandleOnDevice(int device, DWORD access);
HANDLE getHandleOnVolume(int volume, DWORD access);
QString getDriveLabel(const char *drv);
bool getLockOnVolume(HANDLE handle);
bool removeLockOnVolume(HANDLE handle);
bool unmountVolume(HANDLE handle);
bool isVolumeUnmounted(HANDLE handle);
char *readSectorDataFromHandle(HANDLE handle, unsigned long long startsector, unsigned long long numsectors, unsigned long long sectorsize);
bool writeSectorDataToHandle(HANDLE handle, char *data, unsigned long long startsector, unsigned long long numsectors, unsigned long long sectorsize);
unsigned long long getNumberOfSectors(HANDLE handle, unsigned long long *sectorsize);
unsigned long long getFileSizeInSectors(HANDLE handle, unsigned long long sectorsize);
bool spaceAvailable(char *location, unsigned long long spaceneeded);
bool checkDriveType(char *name, ULONG *pid);

#endif // DISK_H
