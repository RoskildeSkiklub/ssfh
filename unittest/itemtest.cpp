#include "itemtest.h"

// The class to test
#include "item.h"

// Stuff to handle the testdatabase
#include "unittestdb.h"

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
}

void ItemTest::constructWithArguments() {
    UnitTestDB::closeDB();
    QVERIFY2( UnitTestDB::resetDB(), "Unable to open test database" );
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

}


void ItemTest::insert()
{
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
    Item item3 = Item( "id3", "type1", "size1", "mark1", "model1", "year1", "cond1", 1, "rent1", "note1" );
    try {
        item1.db_insert();
        QFAIL( "Restrains in database failed for items with illegal type, rentalgroup" );
    } catch ( ... ) {
        QVERIFY2( true, "Item with illegal type, rentalgroup, could not be inserted" );
    }

}

void ItemTest::cleanupTestCase()
{
}
