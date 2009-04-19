TEMPLATE = app
TARGET = ..\..\Win32DiskImager
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += release
DEFINES -= UNICODE

# Input
HEADERS += disk.h mainwindow.h
FORMS += mainwindow.ui
SOURCES += disk.cpp main.cpp mainwindow.cpp
RESOURCES += gui_icons.qrc
RC_FILE += DiskImager.rc
