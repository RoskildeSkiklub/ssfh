-- Note, some values in this file is encoded in utf8.

-- Some default items (skis)
insert or replace into items values ( '1', 'ski', '174', 'Rossignol', 'Scratch Eco', '~2007', 'used', 2100, 'adult', 'in',  'Mads''s ski' );
insert or replace into items values ( '2', 'ski', '152', 'Salomon',   '',            '~2008', 'used', 3000, 'adult', 'in',  'Connies ski' );
insert or replace into items values ( '3', 'ski', '131', 'Salomon',   'Fish',        '~2006', 'used',  600, 'child',  'in', 'Baltazars ski' );
insert or replace into items values ( '4', 'ski', '100', 'Dynastar',  '',            '2009',  'used', 1200, 'child',  'in', 'Matildes ski' );
insert or replace into items values ( '5', 'ski', '185', 'Armada',    'JJ',          '2012',  'new',  7000, 'adult',  'in', 'Upcoming ski' );

-- Some default items (boots)
insert or replace into items values ( '6', 'boots', '45', 'Salomon', 'Comfy Fit', '2007', 'used', 1695, 'accessory', 'in', 'Mads''s boots' );
insert or replace into items values ( '7', 'boots', '42', 'Salomon', NULL,        '2006', 'used', 1595, 'accessory', 'in', 'Connies boots' );
insert or replace into items values ( '8', 'boots', '41', NULL,      NULL,        NULL,   'used',  400, 'accessory', 'in', 'Baltazars boots' );
insert or replace into items values ( '9', 'boots', '31', NULL,      NULL,        NULL,   'used',  600, 'accessory', 'in', 'Matildes boots' );

-- Some default hirers
insert or replace into hirers values( 0, '885', '',           'Mads Bondo',   'Dydensborg', 'Bakkekammen 51', '3600', 'Frederikssund', 'Denmark', 'Mads himself' );
insert or replace into hirers values( 1, '',    '',           'Connie Bondo', 'Dydensborg', 'Bakkekammen 51', '3600', 'Frederikssund', 'Denmark', 'Mads wife' );
insert or replace into hirers values( 2, '',    '1231231230', 'Joe Doe',      'Boe',        'Lærkevej 42',    '2100', 'København Ø',   'Denmark', 'A hirer with æøå ÆØÅ' );


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
-- insert or replace into contractitems values( 6, 1, '142', 'out', 'accessory', 0, 'lejer mine egne boots' );
-- insert or replace into contractitems values( 7, 1, '143', 'out', 'accessory', 0, 'lejer Connies boots' );
-- insert or replace into contractitems values( 8, 1, '144', 'out', 'accessory', 0, 'lejer Baltazars boots ' );
-- insert or replace into contractitems values( 9, 1, '145', 'out', 'accessory', 0, 'lejer Matildes boots' );
 
-- insert or replace into itemevents values( 8, '42', '2010-06-26 12:01:15', 'handed out', '' );
-- insert or replace into itemevents values( 9, '43', '2010-06-26 12:01:20', 'handed out', '' );
-- insert or replace into itemevents values( 10, '44', '2010-06-26 12:01:22', 'handed out', '' );
-- insert or replace into itemevents values( 11, '45', '2010-06-26 12:01:25', 'handed out', '' );
-- insert or replace into itemevents values( 12, '142', '2010-06-26 12:01:26', 'handed out', '' );
-- insert or replace into itemevents values( 13, '143', '2010-06-26 12:01:30', 'handed out', '' );
-- insert or replace into itemevents values( 14, '144', '2010-06-26 12:01:35', 'handed out', '' );
-- insert or replace into itemevents values( 15, '145', '2010-06-26 12:01:40', 'handed out', '' );

