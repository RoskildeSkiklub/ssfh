include(../common.pri)

QT += testlib sql phonon
# QT -= gui
TARGET = unittest
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
INCLUDEPATH += ../src/
SOURCES += itemtest.cpp main.cpp unittestdb.cpp \
    contracttest.cpp \
    utilitytest.cpp
HEADERS += AutoTest.h itemtest.h \
    unittestdb.h \
    contracttest.h \
    utilitytest.h \
    unittest_utilities.h

LIBS += ../src/objs/item.o \
../src/objs/log.o \
../src/objs/db_consts.o \
../src/objs/utility.o \
../src/objs/contract.o \
../src/objs/hirer.o \
../src/objs/contractitem.o \
../src/objs/posprinter.o \
../src/objs/posimage.o \
../src/objs/globals.o \
../src/objs/feedbackobject.o \
../src/objs/moc_feedbackobject.o
