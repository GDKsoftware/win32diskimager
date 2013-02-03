Image Writer for Microsoft Windows
Release 0.7 - Sourceforge Edition
======
About:
======
This utility is used to write img files to SD and USB cards.
Simply run the utility, point it at your img, and then select the
removable device to write to.

This utility can not write CD-ROMs.

Future releases and source code available on our Sourceforge project:

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
This version adds the ability to copy the MD5sum to the clipboard for use in other
apps.  When reading from an SD card, make sure the "MD5 Hash" checkbox is checked, and
after the image is read, it will generate an MD5sum for you.  Click on the hash, and
press <ctrl>-c or right-click on it and select Copy to save it to the clipboard.

Also of note, the program now defaults to the USERPROFILE Downloads directory for
saving/loading images.  Due to a change in behavior from XP to Windows 7, to create a new
image, it is now necessary to type the filename into the file window (as opposed to using
the folder button to select a file).  If only a file name is typed, the program will save it
as follows (using test.img as an example):
Windows XP:  C:\Documents and Settings\<USER>\My Documents\Downloads\test.img
Windows 7:   C:\Users\<USER>\Downloads\test.img
(Note:  <USER> is the login name of the current user)


Legal:
Image Writer for Windows is licensed under the General Public
License v2. The full text of this license is available in 
GPL-2.

This project uses and includes binaries of the MinGW runtime library,
which is in the public domain.

This project uses and includes binaries of the Qt library from 
http://www.qtsoftware.com/, licensed under the Library General Public 
license. It is available at http://qt-project.org

The license text is available in LGPL-2.1

Original version developed by Justin Davis <tuxdavis@gmail.com>
Maintained by the ImageWriter developers (https://launchpad.net/~image-writer-devs).

