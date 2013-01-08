TEMPLATE = app
TARGET = ../../Win32DiskImager
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += release
DEFINES -= UNICODE
DEFINES += QT_NO_CAST_FROM_ASCII

# Input
HEADERS += disk.h mainwindow.h
FORMS += mainwindow.ui
SOURCES += disk.cpp main.cpp mainwindow.cpp
RESOURCES += gui_icons.qrc
RC_FILE += DiskImager.rc
TRANSLATIONS  = diskimager_es.ts

OTHER_FILES += \
    diskimager_es.ts
