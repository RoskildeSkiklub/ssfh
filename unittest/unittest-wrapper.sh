#!/bin/bash

TMPFILE=`mktemp -t`
./unittest "$@" > $TMPFILE
RET=$?
./unittest-output-to-error.pl < $TMPFILE
exit $RET