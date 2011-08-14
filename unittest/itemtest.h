#ifndef ITEMTEST_H
#define ITEMTEST_H

#include "AutoTest.h"
#include <QObject>

class ItemTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void constructDefault();
    void constructWithArguments();
    void insert();
    void cleanupTestCase();
};

DECLARE_TEST(ItemTest)

#endif // ITEMTEST_H
