#include "itemtest.h"

// The class to test
#include "item.h"

void ItemTest::initTestCase()
{
}

void ItemTest::constructDefault()
{
    Item item = Item();
    QCOMPARE( item.getId(), QString("") );
    QCOMPARE( item.getRentalGroup(), QString("") );
    QCOMPARE( item.getState(), QString("") );
    QCOMPARE( item.getType(), QString("") );
}

void ItemTest::constructWithArguments() {
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


void ItemTest::test2()
{
    // QVERIFY(1 == 0);
}

void ItemTest::cleanupTestCase()
{
}
