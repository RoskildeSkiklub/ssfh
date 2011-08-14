#ifndef CONTRACTTEST_H
#define CONTRACTTEST_H

#include "AutoTest.h"
#include <QObject>

class ContractTest : public QObject
{
    Q_OBJECT
private slots:
    // void initTestCase();
    void constructDefault();
    void constructWithArguments();
    void insert();
    // void cleanupTestCase();

};

DECLARE_TEST(ContractTest)

#endif // CONTRACTTEST_H
