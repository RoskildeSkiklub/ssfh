
include(../common.pri)

QT += sql multimedia widgets
TARGET = SnowStuffForHire.bin
TEMPLATE = app

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
    selectcontractdialog.cpp \
    qusb.cpp \
    checksetupdialog.cpp \
    creditcard.cpp \
    feedbackobject.cpp \
    swapdialog.cpp \
    reiddialog.cpp \
    printiddialog.cpp \
    performstatusdialog.cpp \
    printerhelpers.cpp \
    stockstatusdialog.cpp \
    trackitemdialog.cpp
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
    selectcontractdialog.h \
    qusb.h \
    checksetupdialog.h \
    creditcard.h \
    feedbackobject.h \
    swapdialog.h \
    reiddialog.h \
    printiddialog.h \
    performstatusdialog.h \
    printerhelpers.h \
    stockstatusdialog.h \
    trackitemdialog.h
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
    selectcontractdialog.ui \
    checksetupdialog.ui \
    swapdialog.ui \
    reiddialog.ui \
    printiddialog.ui \
    performstatusdialog.ui \
    stockstatusdialog.ui \
    trackitemdialog.ui
TRANSLATIONS = SnowStuffForHire_da_DK.ts

# Keep moc and stuff out of the way - if possible
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs

################################################################################
# USB SUPPORT
# I would like to be able to, at some point, check the setup.
# This involves enumerating the USB devices connected to the system.
# Under Linux, we can check for the libusb package.


# Support for usb (libusb, even under Windows?) is always
# needed, in order to support device enumeration/detection.
# Note, the code may not actually compile under Windows because of this...
unix {
    message( "Under Linux/Unix. Checking if libusb have been installed using pkgconfig." )
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb
    DEFINES += HAVE_USB
    message( "Adding support for libusb" )
}

# At some point this may be a better solution. (Change to libusb, of course)
#packagesExist(glib-2.0) {
#    DEFINES += HAS_GLIB
#    PKGCONFIG += glib-2.0
#}


################################################################################
# Setup for Pertelian support (display)
# mbd: 2011-10-19, postponing this somewhat.
# CONFIG += pertelian
# Pertelian support under Linux
pertelian {
    DEFINES+=HAVE_PERTELIAN
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lPertelianLib2
    message("Adding support for Pertelian from /usr/local")
}


# usb support should work with both Windows and Linux, if you use libusb...
# Obviously, you need a different linker path under Windows...
# Perhaps this could use PKGCONFIG under Linux... 
#usb {
#    DEFINES+=HAVE_USB
#    LIBS += -lusb
#    message( "Adding support for libusb" )
#} 


################################################################################
# The version passed to the compiles...
DEFINES+= VERSION=\\\"$$VERSION\\\"

# Resources
RESOURCES     = ../SnowStuffForHire.qrc
