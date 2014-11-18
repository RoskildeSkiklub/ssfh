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