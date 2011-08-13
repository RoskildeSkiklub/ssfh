#include "itemtest.h"

// The class to test
#include "item.h"

void ItemTest::initTestCase()
{
}

void ItemTest::constructDefault()
{
    Item item = Item();
    // QVERIFY(item.getId)


    QVERIFY(1 + 1 == 2);
}

void ItemTest::test2()
{
    QVERIFY(1 == 0);
}

void ItemTest::cleanupTestCase()
{
}
