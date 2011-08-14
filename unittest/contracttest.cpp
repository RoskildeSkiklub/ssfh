#include "contracttest.h"

#include "contract.h"
#include "unittestdb.h"
#include "db_consts.h"

void ContractTest::constructDefault() {
    // Default should not do anything in the database
    UnitTestDB::closeDB();
    Contract contract;
    QCOMPARE( contract.getId(), (long long) -1 );
    QCOMPARE( contract.getState(), DB::Contract::State::booking );
}

void ContractTest::constructWithArguments() {

}

void ContractTest::insert() {

}
