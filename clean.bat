@echo off
cd src
PATH C:\Qt\4.4.1\bin;C:\MinGW\bin;%PATH%
C:\MinGW\bin\mingw32-make.exe release-distclean debug-distclean
cd ..
pause
@echo on