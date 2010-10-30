-- Setting various rules for Roskilde Skiklub Hedeland. 
-- These are price ranges, stuff like that.
-- Note, that these are set up in Danish, using the Danish lanuage.
-- Encoding of this file should be utf8.

-- ----------------------------------------------------------------------
-- RENTAL GROUPS
-- These are the three rentalgroups we work with, and their alternatives.
insert into rentalgroups values( 'børneski', 'voksenski' );
insert into rentalgroups values( 'voksenski', 'børneski' );
insert into rentalgroups values( 'tilbehør', null );

-- ----------------------------------------------------------------------
-- DAY GROUPS
-- We only work with two groups
insert into daygroups values( 'mandag-fredag' );
insert into daygroups values( 'weekend' );

-- ----------------------------------------------------------------------
-- PRICE RULES

-- Adult items are full price
insert into pricerules values( 'voksenski', 'mandag-fredag', 0, 3, 50, null );
insert into pricerules values( 'voksenski', 'weekend', 0, 3, 80, null );
insert into pricerules values( 'voksenski', 'mandag-fredag', 3, 24, 100, null );
insert into pricerules values( 'voksenski', 'weekend', 3, 24, 200, null );
insert into pricerules values( 'voksenski', null, 24, 48, 140, null );
insert into pricerules values( 'voksenski', null, 48, null, 140, 60 );

-- Child items are reduced price
insert into pricerules values( 'børneski', 'mandag-fredag', 0, 3, 25, null );
insert into pricerules values( 'børneski', 'weekend', 0, 3, 40, null );
insert into pricerules values( 'børneski', 'mandag-fredag', 3, 24, 50, null );
insert into pricerules values( 'børneski', 'weekend', 3, 24, 75, null );
insert into pricerules values( 'børneski', null, 24, 48, 140, null );
insert into pricerules values( 'børneski', null, 48, null, 140, 60 );

-- Supplements are free. Always.
insert into pricerules values( 'tilbehør', null, 0, null, 0, 0 );

-- ----------------------------------------------------------------------
-- ITEM TYPES
insert into itemtypes values( 'ski' );
insert into itemtypes values( 'snowboard' );
insert into itemtypes values( 'støvler' );
insert into itemtypes values( 'hjelm' );
insert into itemtypes values( 'stave' );

