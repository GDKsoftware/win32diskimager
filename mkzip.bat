@echo off
PATH C:\Program Files\WinZip;c:\Program Files\Bazaar;%PATH%
bzr export win32diskimager-source.zip
wzzip win32diskimager-binary.zip mingwm10.dll Win32DiskImager.exe QtGui4.dll QtCore4.dll GPL-2 LGPL-2.1 README.txt
@echo on