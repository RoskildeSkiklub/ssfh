#include "itemtest.h"

// Qt
#include <QSqlQuery>

// The class to test
#include "item.h"

// Other stuff from the app we need
#include "log.h"
#include "contract.h"
#include "hirer.h"
#include "utility.h"
#include "db_consts.h"
#include "exception.h"

// Stuff to handle the testdatabase
#include "unittestdb.h"
#include "unittest_utilities.h"


void ItemTest::initTestCase()
{
}

void ItemTest::constructDefault()
{
    UnitTestDB::closeDB();
    Item item = Item();
    QCOMPARE( item.getId(), QString("") );
    QCOMPARE( item.getRentalGroup(), QString("") );
    QCOMPARE( item.getState(), QString("") );
    QCOMPARE( item.getType(), QString("") );
    QVERIFY2( UnitTestDB::isClosedDB(), "Database not closed after test" );
}

void ItemTest::constructWithArguments() {
    UnitTestDB::closeDB();
    Item item1 = Item( "id1", "type1", "size1", "mark1", "model1", "year1", "cond1", 1, "rent1", "note1" );
    QCOMPARE( item1.getId(), QString( "id1" ) );
    QCOMPARE( item1.getRentalGroup(), QString("rent1") );
    QCOMPARE( item1.getState(), QString("in") ); // Default constructor sets to in
    QCOMPARE( item1.getType(), QString("type1") );

    Item item2 = Item( "id2", "type2", "size2", "mark2", "model2", "year2", "cond2", 2, "rent2", "note2" );
    QCOMPARE( item2.getId(), QString( "id2" ) );
    QCOMPARE( item2.getRentalGroup(), QString("rent2") );
    QCOMPARE( item2.getState(), QString("in") ); // Default constructor sets to in
    QCOMPARE( item2.getType(), QString("type2") );

    QVERIFY2( UnitTestDB::isClosedDB(), "Database not closed after test" );
}


void ItemTest::insert()
{
    QVERIFY2( UnitTestDB::resetDB( "ItemTest_insert" ), "Unable to open test database" );
    // The test database contains some type definitions of type and rentalgroup
    // First, check that a valid item can be inserted
    Item item1 = Item( "id1", "ski", "size1", "mark1", "model1", "year1", "cond1", 1, "child", "note1" );
    item1.db_insert();
    QVERIFY( true );

    // Can not insert with same id, that is, insert again
    Item item2 = Item( "id2", "ski", "size1", "mark1", "model1", "year1", "cond1", 1, "child", "note1" );
    try {
        item1.db_insert();
        QFAIL( "Duplicate item inserted." );
    } catch( ... ) {
        QVERIFY2( true, "Duplicate id not inserted" );
    }

    item2 = Item( "id2", "type1", "size1", "mark1", "model1", "year1", "cond1", 1, "rent1", "note1" );
    try {
        item1.db_insert();
        QFAIL( "Restrains in database failed for items with illegal type, rentalgroup" );
    } catch ( ... ) {
        QVERIFY2( true, "Item with illegal type, rentalgroup, could not be inserted" );
    }
    Item item3 = Item( "id3", "type1", "size1", "mark1", "model1", "year1", "cond1", 1, "rent1", "note1" );    try {
        item1.db_insert();
        QFAIL( "Restrains in database failed for items with illegal type, rentalgroup" );
    } catch ( ... ) {
        QVERIFY2( true, "Item with illegal type, rentalgroup, could not be inserted" );
    }
    UnitTestDB::closeDB();
}

void ItemTest::cleanupTestCase()
{
}

void ItemTest::db_reid() {
    Log::Logger log("void ItemTest::db_reid()");
    QVERIFY2( UnitTestDB::resetDB( "ItemTest_reid" ), "Unable to open test database" );
    // Need some stuff to test with.
    // This must perform two rentals, then reid one item and check that there are no problems.

    qlonglong secondContractId = -1;

    {
        // Create the first contract.
        Contract contract1;
        QCOMPARE( contract1.getState(), DB::Contract::State::booking );
        Hirer hirer1( 1 );
        contract1.setHirer( hirer1 );
        contract1.addItem( "1" );
        contract1.addItem( "2" );
        contract1.addItem( "3" );
        contract1.activate();
        QCOMPARE( contract1.getState(), DB::Contract::State::active );
        QCOMPARE( contract1.getId(), (long long) 1 );
        // Return the first item
        contract1.returnItem( "1" );
        QCOMPARE( contract1.getState(), DB::Contract::State::active );
    }

    {
        // Create next contract
        Contract contract2;
        QCOMPARE( contract2.getState(), DB::Contract::State::booking );
        Hirer hirer2( 2 );
        contract2.setHirer( hirer2 );
        contract2.addItem( "1" );
        contract2.addItem( "4" );
        contract2.addItem( "5" );
        contract2.activate();
        QCOMPARE( contract2.getState(), DB::Contract::State::active );
        QCOMPARE( contract2.getId(), (long long) 2 );
        secondContractId = contract2.getId();
    }

    // Now, Check there is no item 424242 and 424243
    QVERIFY_THROW( Item::db_load( "424242" ), Exception );
    QVERIFY_THROW( Item::db_load( "424243" ), Exception );

    // Reid from 424242 to 424243 should fail
    QVERIFY_THROW( Item::db_reid( "424242", "424243" ), Exception );
    // Reid from 424242 to 1 should fail
    QVERIFY_THROW( Item::db_reid( "424242", "1"), Exception );

    // Reid from 1 to 2 should fail
    QVERIFY_THROW( Item::db_reid( "1", "2"), Exception );

    {
        // Check that there are no contractlines and itemevents with for item 1
        QSqlQuery query;
        query_check_prepare( query, "select count(*) from itemevents where item_id = '1'" );
        query_check_exec( query );
        query_check_first( query );
        int num = query.value( 0 ).toInt();
        QVERIFY( num > 0 );
        query_check_prepare( query, "select count(*) from contractitems where item_id = '1'" );
        query_check_exec( query );
        query_check_first( query );
        num = query.value( 0 ).toInt();
        QVERIFY( num > 0 );
    }

    // Now, reid 1 => 424242
    Item::db_reid( "1", "424242" );
    // Make sure 424242 exists and 1 does not
    Item::db_load( "424242" );
    QVERIFY_THROW( Item::db_load( "1" ), Exception );

    int numItemEvents424242;
    {
        // Check that there are no contractlines and itemevents with for item 1
        QSqlQuery query;
        query_check_prepare( query, "select count(*) from itemevents where item_id = '1'" );
        query_check_exec( query );
        query_check_first( query );
        int num = query.value( 0 ).toInt();
        QVERIFY( num == 0 );
        query_check_prepare( query, "select count(*) from contractitems where item_id = '1'" );
        query_check_exec( query );
        query_check_first( query );
        num = query.value( 0 ).toInt();
        QVERIFY( num == 0 );
        // And, now check there are for 424242
        query_check_prepare( query, "select count(*) from itemevents where item_id = '424242'" );
        query_check_exec( query );
        query_check_first( query );
        num = query.value( 0 ).toInt();
        QVERIFY( num > 1 );
        numItemEvents424242 = num;
        query_check_prepare( query, "select count(*) from contractitems where item_id = '424242'" );
        query_check_exec( query );
        query_check_first( query );
        num = query.value( 0 ).toInt();
        QVERIFY( num > 1 );
    }

    // Now, to just make sure, rename to 424243
    Item::db_reid( "424242", "424243" );
    // Make sure 424243 exists and 424242 does not.
    Item::db_load( "424243" );
    QVERIFY_THROW( Item::db_load( "424242" ), Exception );
    {
        // Check that there are no contractlines and itemevents with for item 424242
        QSqlQuery query;
        query_check_prepare( query, "select count(*) from itemevents where item_id = '424242'" );
        query_check_exec( query );
        query_check_first( query );
        int num = query.value( 0 ).toInt();
        QVERIFY( num == 0 );
        query_check_prepare( query, "select count(*) from contractitems where item_id = '424242'" );
        query_check_exec( query );
        query_check_first( query );
        num = query.value( 0 ).toInt();
        QVERIFY( num == 0 );
        // And, now check there are for 424243
        query_check_prepare( query, "select count(*) from itemevents where item_id = '424243'" );
        query_check_exec( query );
        query_check_first( query );
        num = query.value( 0 ).toInt();
        QVERIFY( num == numItemEvents424242 + 1 ); // Must be one more...
        query_check_prepare( query, "select count(*) from contractitems where item_id = '424243'" );
        query_check_exec( query );
        query_check_first( query );
        num = query.value( 0 ).toInt();
        QVERIFY( num > 1 );

    }
    // Now check that we can deliver back on the first contract.
    // Handing back 1 should fail, wheres 424243 should be OK
    Contract contract2reloaded = Contract::db_load( secondContractId );
    QVERIFY_THROW( contract2reloaded.returnItem( "1" ), Exception );
    contract2reloaded.returnItem( "424243" );
    UnitTestDB::closeDB();
}

