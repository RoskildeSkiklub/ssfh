# Toplevel pro file
include(common.pri)

TEMPLATE = subdirs
SUBDIRS = src unittest doc
CONFIG += ordered

# Will be doing some dist work later on
MY_DOCFILES=Doxyfile Logging.txt NotesOnSundhedsKort.txt Notes.txt pertelian-display.rules PertelianNote.txt PosPrinter.txt postnr.website postnr.xls Requirements.txt
# This needs fixing... 
MY_SCHEMAFILES=Makefile create-db-consts.pl postnr-to-csv.pl schema.sql rsh-setup.sql test-data.sql Transactions.txt unittest-data.sql unittest-setup.sql \
               db-46-to-47/README db-46-to-47/update.46.47.sh \
               db-47-to-48/README db-47-to-48/update.47.48.sh \
               db-48-to-49/README db-48-to-49/update.48.49.sh
MY_SOUNDFILES=critical_error.wav  error.wav  hirer_set.wav  item_added.wav
MY_GUITESTFILES=guitest.odt reid-dialog/fail.test reid-dialog/ok.test
MY_UNITTESTFILES=unittest-output-to-error.pl unittest-wrapper.sh
DISTFILES = \
    SnowStuffForHire.pro doc/doc.pro unittest/unittest.pro SnowStuffForHire.qrc common.pri \
    make-dist.sh Makefile.l10n Makefile.noqtcreator \
    SnowStuffForHire.pro.user \
    $$join( MY_DOCFILES, " doc/", "doc/" ) \
    $$join( MY_GUITESTFILES, " guitest/", "guitest/" ) \
    $$join( MY_SCHEMAFILES, " schema/", "schema/" ) \
    $$join( MY_SOUNDFILES, " sounds/", "sounds/" ) \
    $$join( MY_UNITTESTFILES, " unittest/", "unittest/" ) \
    src/SnowStuffForHire src/src.pro \
    extras/PertelianLib2-1.09.zip \
    logos/rsh-logo-bw.png \
    ChangeLog

dist.target = dist
dist.commands = ./make-dist.sh $$VERSION $$DISTFILES src/*.h src/*.cpp src/*.ui src/*.ts unittest/*.h unittest/*.cpp
QMAKE_EXTRA_TARGETS += dist



# http://www.3dh.de/3dh.de/2006/12/19/qt-automated-unit-tests-with-qmake/
