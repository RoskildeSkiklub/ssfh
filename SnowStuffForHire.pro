# Toplevel pro file
include(common.pri)

TEMPLATE = subdirs
SUBDIRS = src unittest doc
CONFIG += ordered

# Will be doing some dist work later on
MY_DOCFILES=NotesOnSundhedsKort.txt Notes.txt postnr.website postnr.xls PertelianNote.txt PosPrinter.txt pertelian-display.rules
MY_SCHEMAFILES=Makefile create-db-consts.pl postnr-to-csv.pl schema.sql rsh-setup.sql test-data.sql Transactions.txt
MY_SOUNDFILES=critical_error.wav  error.wav  hirer_set.wav  item_added.wav
DISTFILES = \
    SnowStuffForHire.pro SnowStuffForHire.qrc common.pri make-dist.sh Makefile.l10n \
    $$join( MY_DOCFILES, " doc/", "doc/" ) \
    $$join( MY_SCHEMAFILES, " schema/", "schema/" ) \
    $$join( MY_SOUNDFILES, " sounds/", "sounds/" ) \
    src/SnowStuffForHire src/src.pro \
    extras/PertelianLib2-1.09.zip \
    logos/rsh-logo-bw.png \
    ChangeLog

dist.target = dist
dist.commands = ./make-dist.sh $$VERSION $$DISTFILES src/*.h src/*.cpp src/*.ui src/*.ts
QMAKE_EXTRA_TARGETS += dist



# http://www.3dh.de/3dh.de/2006/12/19/qt-automated-unit-tests-with-qmake/
