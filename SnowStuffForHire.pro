# -------------------------------------------------
# Project created by QtCreator 2010-05-08T22:29:54
# -------------------------------------------------
QT += sql
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
    printchecklistdialog.cpp
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
    printchecklistdialog.h
FORMS += mainwindow.ui \
    rentdialog.ui \
    stockdialog.ui \
    hirerdialog.ui \
    addnewitemdialog.ui \
    returndialog.ui \
    viewhireditemsdialog.ui \
    printchecklistdialog.ui
TRANSLATIONS = SnowStuffForHire_da_DK.ts

# Extra files to include in dist
DISTFILES = doc/NotesOnSundhedsKort.txt \
    doc/Notes.txt \
    doc/postnr.website \
    doc/postnr.xls \
    schema/Makefile \
    schema/create-db-consts.pl \
    schema/postnr-to-csv.pl \
    schema/schema.sql \
    schema/rsh-setup.sql \
    schema/test-data.sql \
    schema/Transactions.txt \
    Makefile.l10n \
    SnowStuffForHire

# The version
VERSION = -0.3.0

# Resources
RESOURCES     = SnowStuffForHire.qrc
