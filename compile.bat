@echo off
cd src
qmake.exe
mingw32-make.exe
cd ..
pause
@echo on
