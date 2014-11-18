//
// Snow Stuff For Hire - A POS-like system designed for very quick rental
// operations at small ski resorts
// Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
//
// This file is part of Snow Stuff For Hire.
//
// Snow Stuff For Hire is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Snow Stuff For Hire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
//

#include "utilitytest.h"
// Stuff to handle the testdatabase
#include "unittestdb.h"
#include "unittest_utilities.h"

// Qt
#include <QSqlQuery>

// App
#include "utility.h"
#include "exception.h"


void UtilityTest::check_foreign_keys() {
    // This tests that the database respects foreign key constraints
    UnitTestDB::resetDB( "UtilityTest_check_foreign_keys" );
    QSqlQuery query;
    query_check_prepare( query, "insert or replace into items values ( '1', 'ski', '174', 'Rossignol', 'Scratch Eco', '~2007', 'used', 2100, 'adult', 'in',  'Mads''s ski' );");
    query_check_exec( query );

    // This should fail, because of constraint
    query_check_prepare( query, "insert or replace into items values ( '1', 'ski', '174', 'Rossignol', 'Scratch Eco', '~2007', 'used', 2100, 'adultssss', 'in',  'Mads''s ski' );");
    QVERIFY_THROW( query_check_exec( query ), Exception );
    UnitTestDB::closeDB();
}

void UtilityTest::versionstring_qe() {
    UnitTestDB::closeDB();

    QCOMPARE( ::versionstring_qe( "1.2.3", "1.2.3"), true );
    QCOMPARE( ::versionstring_qe( "1.0.1", "1.0.0"), true );
    QCOMPARE( ::versionstring_qe( "2.0.0", "1.2.2"), true );
    QCOMPARE( ::versionstring_qe( "1.2.1", "1.1.3"), true );
    QCOMPARE( ::versionstring_qe( "1.1.4", "1.1.4"), true );
    QCOMPARE( ::versionstring_qe( "1.10.0", "1.9.0"), true );

    QCOMPARE( ::versionstring_qe( "1.1.1", "1.1.2"), false );
    QCOMPARE( ::versionstring_qe( "1.1.1", "1.2.1"), false );
    QCOMPARE( ::versionstring_qe( "1.1.5", "1.2.4"), false );
    QCOMPARE( ::versionstring_qe( "1.1.1", "2.0.0"), false );
    QCOMPARE( ::versionstring_qe( "1.5.1", "1.10.1"), false );

    QVERIFY2( UnitTestDB::isClosedDB(), "Database not closed after test" );
}
