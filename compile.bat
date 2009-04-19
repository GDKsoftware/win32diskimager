@echo off
cd src
PATH C:\Qt\4.4.1\bin;C:\MinGW\bin;%PATH%
C:\Qt\4.4.1\bin\qmake.exe
C:\MinGW\bin\mingw32-make.exe
cd ..
pause
@echo on