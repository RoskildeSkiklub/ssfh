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
    void activate();
    void insert();

    void hirerManip1();
    void durationManip1();


    void regression_returnAll();

    // void cleanupTestCase();

};

DECLARE_TEST(ContractTest)

#endif // CONTRACTTEST_H
