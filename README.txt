Image Writer for Microsoft Windows
Release 0.8 - Furball Edition
======
About:
======
This utility is used to write img files to SD and USB cards.
Simply run the utility, point it at your img, and then select the
removable device to write to.

This utility can not write CD-ROMs.

Future releases and source code are available on our Sourceforge project:

This program is Beta , and has no warrranty. It may eat your files,
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
Added a Copy button to copy the MD5Sum to the clipboard.

Added an environment variable "DiskImagesDir" check to use as a default 
directory.  This bypasses the search for the "Downloads" directory (which
has been greatly improved over v0.7).

Added drag and drop.  You can now "drag" a file from Windows Explorer and
 "drop" it into the filename text window.

=============
Known Issues:
=============
One outstanding bug affects users with built-in Ricoh based 4in1 SD card
readers on Windows XP and Vista.  This is actively being debugged and will 
be targeted for the next release.  Workarounds include upgrading to 
Windows 7 or using a USB SD card reader.

Other issues is lack of reformat capabilities, lack of file compression
 support, and an installer.  These are being looked into for future releases.

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
Maintained by the ImageWriter developers (https://launchpad.net/~image-writer-devs).

