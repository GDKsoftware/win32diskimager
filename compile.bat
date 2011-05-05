@echo off
cd src
PATH C:\Qt\2010.05\bin;C:\MinGW\bin;%PATH%
C:\Qt\2010.05\bin\qmake.exe
C:\MinGW\bin\mingw32-make.exe
cd ..
pause
@echo on