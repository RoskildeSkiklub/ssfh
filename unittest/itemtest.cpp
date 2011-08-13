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



void ItemTest::test2()
{
    // QVERIFY(1 == 0);
}

void ItemTest::cleanupTestCase()
{
}
