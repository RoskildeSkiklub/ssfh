#!/bin/bash
#
# Snow Stuff For Hire - A POS-like system designed for very quick rental
# operations at small ski resorts
# Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
#
# This file is part of Snow Stuff For Hire.
#
# Snow Stuff For Hire is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Snow Stuff For Hire is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
#

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
echo "insert into configuration values( 'sqlite_min_version', '3.6.19' );" >> $SQLFILE 
echo "update configuration set value='$TOVER' where key='db_version';" >> $SQLFILE
echo "commit;" >> $SQLFILE

# cat $SQLFILE;
cat $SQLFILE | sqlite3 "$DB" || die "Unable to perform upgrade. Check $SQLFILE for script that failed."

# Clean up.
rm $TRIGGERFILE
rm $SQLFILE
echo "Upgrade succesfull, database version is now: $TOVER"