TEMPLATE = app
TARGET = ../../Win32DiskImager
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += release
DEFINES -= UNICODE
#DEFINES += QT_NO_CAST_FROM_ASCII
VERSION = 0.8
VERSTR = '\\"$${VERSION}\\"'
DEFINES += VER=\"$${VERSTR}\"
QMAKE_TARGET_PRODUCT = "Win32 Image Writer"
QMAKE_TARGET_DESCRIPTION = "Image Writer for Windows to write USB and SD images"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2009-2013 Windows ImageWriter Team"

# Input
HEADERS += disk.h mainwindow.h droppablelineedit.h
FORMS += mainwindow.ui
SOURCES += disk.cpp main.cpp mainwindow.cpp droppablelineedit.cpp
RESOURCES += gui_icons.qrc
RC_FILE = DiskImager.rc
TRANSLATIONS  = diskimager_en.ts

