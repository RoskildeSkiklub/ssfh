-- 
-- Snow Stuff For Hire - A POS-like system designed for very quick rental
-- operations at small ski resorts
-- Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
--
-- This file is part of Snow Stuff For Hire.
--
-- Snow Stuff For Hire is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- Snow Stuff For Hire is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
-- 
-- Setting various rules for Roskilde Skiklub Hedeland. 
-- These are price ranges, stuff like that.
-- Note, that these are set up in Danish, using the Danish lanuage.
-- Encoding of this file should be utf8.

-- ----------------------------------------------------------------------
-- RENTAL GROUPS
-- These are the three rentalgroups we work with, and their alternatives.
insert into rentalgroups values( 'childski', 'adultski' );
insert into rentalgroups values( 'adultski', 'childski' );
insert into rentalgroups values( 'accessories', null );

-- ----------------------------------------------------------------------
-- DAY GROUPS
-- We only work with two groups
insert into daygroups values( 'monday-friday' );
insert into daygroups values( 'weekend' );

-- ----------------------------------------------------------------------
-- PRICE RULES

-- Adult items are full price
insert into pricerules values( 'adultski', 'monday-friday', 0, 3, 50, null );
insert into pricerules values( 'adultski', 'weekend', 0, 3, 80, null );
insert into pricerules values( 'adultski', 'monday-friday', 3, 24, 100, null );
insert into pricerules values( 'adultski', 'weekend', 3, 24, 200, null );
insert into pricerules values( 'adultski', null, 24, 48, 140, null );
insert into pricerules values( 'adultski', null, 48, null, 140, 60 );

-- Child items are reduced price
insert into pricerules values( 'childski', 'monday-friday', 0, 3, 25, null );
insert into pricerules values( 'childski', 'weekend', 0, 3, 40, null );
insert into pricerules values( 'childski', 'monday-friday', 3, 24, 50, null );
insert into pricerules values( 'childski', 'weekend', 3, 24, 75, null );
insert into pricerules values( 'childski', null, 24, 48, 140, null );
insert into pricerules values( 'childski', null, 48, null, 140, 60 );

-- Supplements are free. Always.
insert into pricerules values( 'accessories', null, 0, null, 0, 0 );

-- ----------------------------------------------------------------------
-- ITEM TYPES
insert into itemtypes values( 'ski' );
insert into itemtypes values( 'snowboard' );
insert into itemtypes values( 'boots' );
insert into itemtypes values( 'helmet' );
insert into itemtypes values( 'poles' );

