@echo off
PATH C:\Program Files\WinZip;%PATH%
cd ..\
wzzip -r -p Win32DiskImager\win32diskimager-source.zip Win32DiskImager\
cd Win32DiskImager
wzzip win32diskimager-binary.zip mingwm10.dll Win32DiskImager.exe QtGui4.dll QtCore4.dll GPL-2 LGPL-2.1 README.txt
@echo on