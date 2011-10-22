#!/usr/bin/perl

use warnings;
use strict;

my $postnr;
my $by;
my $gade;
my $firma;
my $provins;
my $land;

while( <> ) {
    ( $postnr, $by, $gade, $firma, $provins, $land ) = split( /\t/, $_ );
    if ( "Postnr." eq $postnr ) { next; }
    print "$postnr\t$by\n";
}
