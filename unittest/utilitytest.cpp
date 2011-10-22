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
