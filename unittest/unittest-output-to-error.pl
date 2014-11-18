#!/usr/bin/perl
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

#contracttest.cpp: In member function ‘void ContractTest::hirerManip1()’:
#contracttest.cpp:54:9: error: ‘Contra’ was not declared in this scope
#contracttest.cpp:54:16: error: expected ‘;’ before ‘t’
#contracttest.cpp:55:9: error: ‘contract’ was not declared in this scope
#contracttest.cpp:58:9: error: ‘contract’ was not declared in this scope


use warnings;
use strict;

my $total = "";
while( <> ) {
    $total .= $_;

    if ( m/SKIP\s*: (.*)/ ) {
        my $res = $1;
        while( <> ) {
            $total .= chop($_);
            if ( m/^\s*Loc: \[(.*)\((\d*)\)\]/ ) {
#                print "Got this: $res\n";
                print STDERR "$1:$2:1: warning: $res\n";
                last;
            }
            $res .= $_;
        }
    }

    if ( m/FAIL\!\s*: (.*)/ ) {
        my $res = $1;
        while( <> ) {
            $total .= chop($_);
            if ( m/^\s*Loc: \[(.*)\((\d*)\)\]/ ) {
#                print "Got this: $res\n";
                print STDERR "$1:$2:1: error: $res\n";
                last;
            }
            $res .= $_;
        }
    }
}
print $total;