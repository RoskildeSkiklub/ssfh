#ifndef UTILITYTEST_H
#define UTILITYTEST_H

#include "AutoTest.h"
#include <QObject>

class UtilityTest : public QObject
{
    Q_OBJECT
private slots:
    void check_foreign_keys();

    void versionstring_qe();
};
DECLARE_TEST(UtilityTest)
#endif // UTILITYTEST_H
