#!/usr/bin/perl

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
