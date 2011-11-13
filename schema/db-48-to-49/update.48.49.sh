#!/bin/bash

# Script to upgrade from version 47 to 48

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
FROMVER=48
TOVER=49

# Check that version is right
CURVER=`echo "select value from configuration where key='db_version';" | sqlite3 "$DB"`

test "$FROMVER" == "$CURVER" || die "Version of database is wrong. Expected: $FROMVER, got: $CURVER"

# Copy the db file to another file, and actually make the changes on that
echo "Creating temporary working copy of database"
TMPDB=`mktemp -t` || die "Unable to call mktemp to create temporary databasefile"
cp "$DB" "$TMPDB" || die "Unable to create working copy of $DB"

# Dump all the data from the itemevents table into a file
echo "Dumping itemevent records"
ITEMEVENTS=`mktemp -t` || die "Unable to create tmp file for dumping itemevents"
echo ".bail ON;" > $ITEMEVENTS
echo ".dump itemevents" | sqlite3 $TMPDB | grep "^INSERT INTO" >> $ITEMEVENTS || die "Unable to dump itemevents"

# Drop the table from the old database and recreate it
echo "Altering table"
SQLFILE=`mktemp -t` || die "Unable to call mktemp to create sqlfile"
echo ".bail ON;" > $SQLFILE
echo "drop table itemevents;" >> $SQLFILE
sqlite3 $TMPDB <<EOF
.bail ON;
drop table itemevents;
create table itemevents(
  id integer primary key asc, 
  item_id text not null, -- Note, no reference, as we allow items to be deleted in some cases, but retain the log lines.
  time datetime,
  -- Do not remove the next line, the build system uses it
  -- TRANSLATE:Item
  event text check ( event in ('created', 'booked', 'handed out', 'returned', 'sent for maintenance', 'lost', 'discarded', 'deleted', 'reid', 'status', 'other' ) ), 
  note text
);
create index ix_itemevents_item_id on itemevents( item_id );
update configuration set value='$TOVER' where key='db_version';
EOF
test 0 == $? || die "Unable to run recreate table. No changes written to database"

# Read the data back into the database
echo "Reading itemevents back into database. This may take a while"
cat $ITEMEVENTS | sqlite3 $TMPDB || die "Unable to read dump back into tmp database. No changes written to database"

# Move the tmpdb to db
echo "All changes done, moving temp database to $DB"
mv $TMPDB "$DB" || die "Unable to move temporary database to $DB"

rm $ITEMEVENTS
rm $SQLFILE

echo "Upgrade succesfull, database version is now: $TOVER"

