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
-- schema for ski rental program

-- ----------------------------------------------------------------------
-- CONFIGURATION INFORMATION
-- This is also filled by the creation script, with version information about the database
drop table if exists configuration;
create table configuration(
  key text primary key, -- key, really, but key is probably reserved
  value text
);
-- DB VERSION: REMEMBER TO UPDATE, WHEN CHANGING ANYTHING
insert into configuration values( 'db_version', '49' );
insert into configuration values( 'sqlite_min_version', '3.6.19' );

-- ----------------------------------------------------------------------
-- RENTAL GROUPS

-- All items belongs to a rental group, but can be changed at the time
-- of renting out, to other rentalgroups.
drop table if exists rentalgroups;
create table rentalgroups(
  id text primary key, -- The name of the rental group: childski, ski, newski, whatever
  alternatives -- List of alternatives to this rental group, that is, changable by operator to one of these. "," seperated
);

-- ----------------------------------------------------------------------
-- DAY GROUPS

-- This describes day groups for price rules. Different day groups
-- have different pricerules.
drop table if exists daygroups;
create table daygroups (
  id text primary key -- The name of the daygroup: weekday, weekend, saturday, holiday, whatever
);
-- ----------------------------------------------------------------------
-- PRICE RULES

-- There must be a system to figure out a price for an item.
-- The rules here, uses the rental group, start time and length of the rental
-- to figure out a price "range", the gui then does the final calculations.

-- This is quite tricky.
-- In order to calculate a price, do something like
-- select minLength, maxLength, price, additionalDayPrice from pricerule 
--   where rentalgroup = :rental_group
--   and (startOn = :startOn or startOn = null)
--   and :rental_hour_length => minLength
--   and (:rental_hour_length < maxLength or maxLength = null)
-- Adjust price, rounded
-- If no price found: do error.
-- if maxLength = null and additionalDayPrice != null then
--   price := price + ceiling((:rental_hour_length - minLength)/24.0)*additionalDayPrice
drop table if exists pricerules;
create table pricerules(
  rentalGroup text references rentalgroups( id ) not null, -- todo: foreign key på rentalgroup
  startOn text references daygroups( id ), -- Different rules, depending on the start day. May be null to match all days.
  minLength integer not null, --min length in hours >=
  maxLength integer, --max length in hours <
  price integer not null, --Price in currency
  additionalDayPrice integer --Price to add for every 24 hours longer than minLength. 0 for most.
);


-- TODO: In gui test for completenes: For every combination of
-- rentalgroup, startOn, and length > 0, test that exactly one record is retrieved
-- Start by selecting
-- select max( minLength ) from pricerule where rentalgroup = :rentalgroup
-- Or something. 
-- TODO: This will need unittesting. I think.

-- ----------------------------------------------------------------------
-- ITEM TYPES

-- Items must be of some sort of type, such as ski, boot, whatever.

-- For items, we need a type
drop table if exists itemtypes;
create table itemtypes(
  id text primary key -- The name of the rental type: skis, snowboard, boots, helmet, poles, etc.
);

-- ----------------------------------------------------------------------
-- ITEMS

-- Finally, the actual items. 

-- Describe all items potentially available for rent.
drop table if exists items;
create table items(
  id text primary key, -- The stuff on the labels acts as an id
  type text references itemtypes( id ) not null,
  size text, -- size or length of item
  mark text, --say Rossignol, salomon, etc
  model text, --say fish, eco, whatever
  year text, -- year of production, < ~ can be used too.
  condition text, --mint, average, trashed
  price integer, --price when bought
  rentalgroup text references rentalgroups( id ) not null, -- Generic DEFAULT rental group. This can be changed on a pr. rental basis.
  -- Do not remove the next line, the build system uses it
  -- TRANSLATE:Item
  state text check( state in ( 'in', 'booked', 'out', 'maintenance', 'lost', 'discarded' ) ),
  note text
);


-- Describe people that can hire
-- Note, the many null rules is required, because the hirer dialogs select
-- statement does not work well, if there are many nulls. 
-- Because we have complete control of the creation of hirers, nulls are not allowed.
-- emptry strings are used to indicate no value.
drop table if exists hirers;
create table hirers(
  id integer primary key asc,
  memberId text not null, -- local member number, if applicable
  ssn text not null, --cpr in dk.
  firstName text not null,
  lastName text not null,
  streetAddress text not null,
  zip text not null,
  city text not null,
  country text not null,
  note text not null
);


-- Contracts tie hirers to items and rental periods using contractitems
drop table if exists contracts;
create table contracts(
  id integer primary key asc,
  hirer integer references hirers( id ) not null,
  creationTime datetime, -- The date the contract was created
  startTime datetime, -- The start of the rental period
  endTime datetime, -- The end of the rental period
  discount integer, -- Any discount, in same unit as price
  price integer, -- The total price of the contract, the sum of the contract lines - discount
  payed integer, -- The amount payed (so far) by the hirer
  -- Do not remove the next line, the build system uses it
  -- TRANSLATE:Contract
  state text check( state in ( 'booking', 'parked', 'active', 'closed' ) ),
  note text
);
-- Index to look up contract from hirer
create index ix_contracts_hirer on contracts( hirer );


-- Individual items on a contract We need this relation mostly in
-- order to support bookings and prices.  Strictly speaking, RSK does not need
-- bookings. However, I like the flexibility of this. Although it is a
-- bit more cumbersome.
drop table if exists contractitems;
create table contractitems(
  id integer primary key asc,
  contract_id integer references contracts( id ) not null,
  item_id text references items( id ) not null,
  -- Do not remove the next line, the build system uses it
  -- TRANSLATE:ContractItem
  state text check( state in ( 'booked', 'out', 'returned' ) ),
  rentalgroup text references rentalgroups( id ) not null,  
  price integer, -- The calculated price of the item on the contract
  note text
);
-- We need indexes to allow looking up based on contract or item_id,
-- and filtering on state.
create index ix_contractitems_contract_id on contractitems( contract_id );
create index ix_contractitems_item_id on contractitems( item_id );
create index ix_contractitems_state on contractitems( state );
-- And, an unique index on the contract_id, item_id combo
create unique index ix_contractitems_contract_id_item_id on contractitems( contract_id, item_id );

-- History of an item. Sort of a log
drop table if exists itemevents;
create table itemevents(
  id integer primary key asc, 
  item_id text not null, -- Note, no reference, as we allow items to be deleted in some cases, but retain the log lines.
  time datetime,
  -- Do not remove the next line, the build system uses it
  -- TRANSLATE:Item
  event text check ( event in ('created', 'booked', 'handed out', 'returned', 'sent for maintenance', 'lost', 'discarded', 'deleted', 'reid', 'status', 'other' ) ), 
  note text
);

-- Need to be able to look up on item_id
create index ix_itemevents_item_id on itemevents( item_id );

-- ----------------------------------------------------------------------
-- ZIP TO CITY MAPPING
-- This is needed in DK, when scanning the Sundhedskort
drop table if exists zipcity;
create table zipcity(
  zip text primary key,
  city text
);
  