#!/bin/bash

BASENAME=snowstuffforhire

# Script to make a dist file
function die() {
    echo "ERROR: " "$@"
    exit 1
}

function usage() {
    echo "$0 <version> file [file ...]"
    exit 1
}

VERSION=$1
if test "x$VERSION" == "x" ; then
    usage;
fi
shift

TMPDIR=$BASENAME-$VERSION
if test -a "$TMPDIR" ; then
    die "TMPDIR $TMPDIR exists - please move it out of the way"
fi;

mkdir $TMPDIR || die "Unable to create TMPDIR $TMPDIR"
# Copy files into the directory
for var in "$@" ; do
    cp --parent "$var" "$TMPDIR" || die "Unable to do cp --parent \"$var\" \"$TMPDIR\""
done

# Clean out moc files.
rm -Rf "$TMPDIR"/*/moc_*.cpp

# Tar it
tar -zcvf $TMPDIR.tar.gz $TMPDIR || die "Unable to tar $TMPDIR"

# Clean up
rm -Rf $TMPDIR
