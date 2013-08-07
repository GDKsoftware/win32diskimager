Image Writer for Microsoft Windows
Release 0.9 - Unnamed Edition
======
About:
======
This utility is used to write img files to SD and USB cards.
Simply run the utility, point it at your img, and then select the
removable device to write to.

This utility can not write CD-ROMs.

Future releases and source code are available on our Sourceforge project:
http://sourceforge.net/projects/win32diskimager/

This program is Beta , and has no warranty. It may eat your files,
call you names, or explode in a massive shower of code. The authors take
no responsibility for these possible events.

===================
Build Instructions:
===================
Requirements:
1. MinGW (20120426 from http://mingw.org)
2. Qt for Windows SDK (currently using 4.8.4 mingw from http://qt-project.org)

Short Version:
1. Install the Qt Full SDK
2. Run compile.bat -OR- run qmake and then make in the src folder.
3. Compile.bat may be edited to change installation paths of MinGW and QT

=============
New Features:
=============
Added a custom file dialog box to solve the issues between opening a non-existant 
file and saving a read-only file.

Fixed bug LP:1118217 - can not select SD card to write image to on select computers.
Fixed bug LP:1191156 File-Open dialog does not accept non-existing img files as target.

=============
Known Issues:
=============

*  Lack of reformat capabilities.
*  Lack of file compression support
*  Lack of an installer.  

These are being looked into for future releases.

======
Legal:
======
Image Writer for Windows is licensed under the General Public
License v2. The full text of this license is available in 
GPL-2.

This project uses and includes binaries of the MinGW runtime library,
which is available at http://www.mingw.org

This project uses and includes binaries of the Qt library, licensed under the 
"Library General Public License" and is available at 
http://www.qt-project.org/.

The license text is available in LGPL-2.1

Original version developed by Justin Davis <tuxdavis@gmail.com>
Maintained by the ImageWriter developers (http://sourceforge.net/projects/win32diskimager).

