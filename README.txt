Image Writer for Microsoft Windows
Release 0.1 - The "Debugged From A Truck Stop" Release
===
About:
This utility is used to write Ubuntu img files to SD and USB cards.
Simply run the utility, point it at your img, and then select the
removable device to write to.

This utility can not write CD-ROMs.

Future releases and source code available on our Launchpad project:
https://launchpad.net/win32-image-writer

===
Build Instructions:

Requirements:
1. MinGW
2. Qt for Windows SDK

Short Version:
1. Install the Qt Full SDK
2. Run compile.bat -OR- run qmake and then make in the src folder.
3. Compile.bat may be edited to change installation paths of MinGW and QT

===

Legal:
This project uses and includes binaries of the Qt library from 
http://www.qtsoftware.com/, licensed under the Library General Public 
license. It is available at http://get.qtsoftware.com/qtsdk/qt-sdk-win-opensource-2009.01.1.exe