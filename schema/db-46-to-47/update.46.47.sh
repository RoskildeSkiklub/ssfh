#!/bin/bash

# Script to upgrade from one version to another

function die () {
    echo "Error: " "$@"
    exit 1
}

DB=$1
if test "x$DB" == "x" ; then
    echo "Usage: $0 <dbfile>"
    exit 1
fi

test -a "$DB" || die "Unable to read file $DB"
which sqlite3 &> /dev/null || die "Unable to locate binary sqlite3"

# Consts
FROMVER=46
TOVER=47

# Check that version is right
CURVER=`echo "select value from configuration where key='db_version';" | sqlite3 "$DB"`

test "$FROMVER" == "$CURVER" || die "Version of database is wrong. Expected: $FROMVER, got: $CURVER"

TRIGGERFILE=`mktemp -t` || die "Unable to call mktemp"

# Get the triggers from sqlite3
echo "SELECT name FROM sqlite_master where type = 'trigger';" | sqlite3 "$DB" > $TRIGGERFILE

SQLFILE=`mktemp -t` || die "Unable to call mktemp"

# Remove all the triggers
# Make sure sqlite3 bails out on error.
echo ".bail ON;" > $SQLFILE
echo "begin transaction; " >> $SQLFILE
cat $TRIGGERFILE | awk '{print "drop trigger "$0";"}' >> $SQLFILE
echo "update configuration set value='$TOVER' where key='db_version';" >> $SQLFILE
echo "commit;" >> $SQLFILE

# cat $SQLFILE;
cat $SQLFILE | sqlite3 "$DB" || die "Unable to perform upgrade. Check $SQLFILE for script that failed."

# Clean up.
rm $TRIGGERFILE
rm $SQLFILE
echo "Upgrade succesfull, database version is now: $TOVER"

