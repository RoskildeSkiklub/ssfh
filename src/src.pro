
include(../common.pri)

QT += sql
TARGET = SnowStuffForHire.bin
TEMPLATE = app

# Setup for Pertelian support
CONFIG += pertelian

SOURCES += main.cpp \
    mainwindow.cpp \
    rentdialog.cpp \
    stockdialog.cpp \
    log.cpp \
    exception.cpp \
    contract.cpp \
    hirer.cpp \
    item.cpp \
    hirerdialog.cpp \
    utility.cpp \
    interceptor.cpp \
    globals.cpp \
    dksundhedskort.cpp \
    addnewitemdialog.cpp \
    db_consts.cpp \
    contractitem.cpp \
    returndialog.cpp \
    viewhireditemsdialog.cpp \
    posprinter.cpp \
    posimage.cpp \
    printchecklistdialog.cpp \
    massstatechangedialog.cpp \
    dbmaintenance.cpp \
    plaintexteditdialog.cpp \
    selectcontractdialog.cpp
HEADERS += mainwindow.h \
    rentdialog.h \
    stockdialog.h \
    log.h \
    exception.h \
    contract.h \
    hirer.h \
    item.h \
    hirerdialog.h \
    utility.h \
    interceptor.h \
    globals.h \
    dksundhedskort.h \
    addnewitemdialog.h \
    db_consts.h \
    contractitem.h \
    returndialog.h \
    viewhireditemsdialog.h \
    posprinter.h \
    posimage.h \
    printchecklistdialog.h \
    massstatechangedialog.h \
    dbmaintenance.h \
    plaintexteditdialog.h \
    selectcontractdialog.h
FORMS += mainwindow.ui \
    rentdialog.ui \
    stockdialog.ui \
    hirerdialog.ui \
    addnewitemdialog.ui \
    returndialog.ui \
    viewhireditemsdialog.ui \
    printchecklistdialog.ui \
    massstatechangedialog.ui \
    plaintexteditdialog.ui \
    selectcontractdialog.ui
TRANSLATIONS = SnowStuffForHire_da_DK.ts

# Keep moc and stuff out of the way - if possible
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs

 
# Pertelian support under Linux
pertelian {
    DEFINES+=HAVE_PERTELIAN
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lPertelianLib2
    message("Adding support for Pertelian from /usr/local")
}


# The version passed to the compiles...
DEFINES+= VERSION=\\\"$$VERSION\\\"

# Resources
RESOURCES     = ../SnowStuffForHire.qrc
