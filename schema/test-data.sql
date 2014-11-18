-- Actual ski/stuff owned by RSK - the ids are wrong though...
insert or replace into items values ( '242', 'ski',  '185', 'Atomic',   'Arc Sport HV', '~1995', 'brugt', 0, 'voksenski', 'in', 'Doneret af Charlotte Ægidius 2010' );
insert or replace into items values ( '243', 'ski',  '150', 'Dynastar', 'Team X5', '~2000', 'trænger til vedligehold', 0, 'børneski', 'in', 'Doneret af Charlotte Ægidius 2010' );
insert or replace into items values ( '244', 'støvler', '25?', 'Tecno',    'TPS 250', '~2000', 'brugt', 0, 'børneski', 'in', 'Doneret af Charlotte Ægidius 2010' );

-- Mads's stuff ...
insert or replace into items values ( '42', 'ski', '174', 'Rossignol', 'Scratch Eco', '~2007', 'brugt', 2100, 'voksenski', 'in', 'Mads''s ski' );
insert or replace into items values ( '43', 'ski', '152', 'Salomon', '', '~2008', 'brugt', 3000, 'voksenski', 'in', 'Connies ski' );
insert or replace into items values ( '44', 'ski', '131', 'Salomon', 'Fish', '~2006', 'brugt', 600, 'børneski', 'in', 'Baltazars ski' );
insert or replace into items values ( '45', 'ski', '100', 'Dynastar', '', '2009', 'brugt', 1200, 'børneski', 'in', 'Matildes ski' );

insert or replace into items values ( '142', 'støvler', '45', 'Salomon', 'Comfy Fit', '2007', 'brugt', 1695, 'tilbehør', 'in', 'Mads''s støvler' );
insert or replace into items values ( '143', 'støvler', '42', 'Salomon', NULL, '2006', 'brugt', 1595, 'tilbehør','in', 'Connies støvler' );
insert or replace into items values ( '144', 'støvler', '41', NULL, NULL, NULL, 'brugt', 400, 'tilbehør','in', 'Baltazars støvler' );
insert or replace into items values ( '145', 'støvler', '31', NULL, NULL, NULL, 'brugt', 600, 'tilbehør','in', 'Matildes støvler - snart for små til hende.' );

insert or replace into hirers values( 0, '885', '1212701234', 'Mads Bondo', 'Dydensborg', 'Bakkekammen 51', '3600', 'Frederikssund', 'Denmark', 'Mads himself' );
insert or replace into hirers values( 1, '', '', 'Connie Bondo', 'Dydensborg', 'Bakkekammen 51', '3600', 'Frederikssund', 'Denmark', 'Mads kone' );
insert or replace into hirers values( 2, '', '1231231230', 'Joe Doe', 'Boe', 'Lærkevej 42', '2100', 'København Ø', 'Denmark', 'En bruger med æøå ÆØÅ' );
-- Really test stuff, barcode from a CD
insert or replace into items values ( '023942430735', 'ski', '174', 'Rossignol CD', 'Scratch Eco CD', '~2007', 'brugt', 2100, 'voksenski', 'in', 'Mads''s CD' );


-- Create a contract
-- Something done earlier by Mads
-- This reflects the end situation, *not* the events underway...
-- insert or replace into contracts values( 0, 0, '2009-02-02 12:00:00', '2009-02-02 12:00:01', '2009-02-02 15:00:01', 50, 200, 'true', 'closed', 'testnote' );
-- insert or replace into contractitems values( 0, 0, '42', 'returned', 'voksenski', 100, 'lejer mine egne ski' );
-- insert or replace into contractitems values( 1, 0, '44', 'returned', 'børneski', 75, 'lejer Baltazars ski' );

-- insert or replace into itemevents values( 0, '42', '2009-02-02 12:00:00', 'handed out', '' );
-- insert or replace into itemevents values( 1, '44', '2009-02-02 12:00:25', 'handed out', '' );
-- insert or replace into itemevents values( 2, '42', '2009-02-02 14:35:00', 'returned', '' );
-- insert or replace into itemevents values( 3, '44', '2009-02-02 14:36:00', 'returned', '' );

-- insert or replace into itemevents values( 4, '42', '2009-05-25 14:05:00', 'sent for maintenance', '' );
-- insert or replace into itemevents values( 5, '44', '2009-05-25 15:36:00', 'sent for maintenance', '' );
-- insert or replace into itemevents values( 6, '42', '2009-05-30 12:55:00', 'returned', '' );
-- insert or replace into itemevents values( 7, '44', '2009-05-30 14:23:00', 'returned', '' );


-- insert or replace into contracts values( 1, 0, '2009-06-26 11:55:01', '2010-06-26 12:00:01', '2010-06-27 15:00:01', 0, 300, 'true', 'active', 'testnote' );
-- insert or replace into contractitems values( 2, 1, '42', 'out', 'voksenski', 100, 'lejer mine egne ski' );
-- insert or replace into contractitems values( 3, 1, '43', 'out', 'voksenski', 100, 'lejer Connies ski' );
-- insert or replace into contractitems values( 4, 1, '44', 'out', 'voksenski', 100, 'lejer Baltazars ski' );
-- insert or replace into contractitems values( 5, 1, '45', 'out', 'voksenski', 100, 'lejer Matildes ski' ); 
-- insert or replace into contractitems values( 6, 1, '142', 'out', 'tilbehør', 0, 'lejer mine egne støvler' );
-- insert or replace into contractitems values( 7, 1, '143', 'out', 'tilbehør', 0, 'lejer Connies støvler' );
-- insert or replace into contractitems values( 8, 1, '144', 'out', 'tilbehør', 0, 'lejer Baltazars støvler ' );
-- insert or replace into contractitems values( 9, 1, '145', 'out', 'tilbehør', 0, 'lejer Matildes støvler' );
 
-- insert or replace into itemevents values( 8, '42', '2010-06-26 12:01:15', 'handed out', '' );
-- insert or replace into itemevents values( 9, '43', '2010-06-26 12:01:20', 'handed out', '' );
-- insert or replace into itemevents values( 10, '44', '2010-06-26 12:01:22', 'handed out', '' );
-- insert or replace into itemevents values( 11, '45', '2010-06-26 12:01:25', 'handed out', '' );
-- insert or replace into itemevents values( 12, '142', '2010-06-26 12:01:26', 'handed out', '' );
-- insert or replace into itemevents values( 13, '143', '2010-06-26 12:01:30', 'handed out', '' );
-- insert or replace into itemevents values( 14, '144', '2010-06-26 12:01:35', 'handed out', '' );
-- insert or replace into itemevents values( 15, '145', '2010-06-26 12:01:40', 'handed out', '' );

