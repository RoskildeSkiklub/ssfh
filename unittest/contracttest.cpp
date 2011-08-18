#include "contracttest.h"

#include "contract.h"
#include "unittestdb.h"
#include "db_consts.h"

#include "log.h"

void ContractTest::constructDefault() {
    Log::Logger log("void ContractTest::constructDefault()");

    // Default should not do anything in the database
    UnitTestDB::closeDB();
    QVERIFY2( UnitTestDB::isClosedDB(), "Database not closed after closeDB" );
    log.stream() << "Ready for test";

    Contract contract;
    QCOMPARE( contract.getId(), (long long) -1 );
    QCOMPARE( contract.getState(), DB::Contract::State::booking );

    QVERIFY2( UnitTestDB::isClosedDB(), "Database not closed after test" );
}

void ContractTest::activate() {
    Log::Logger log( "void ContractTest::activate()" );
    QVERIFY2( false, "disabled for now");
    return;
    UnitTestDB::closeDB();
    Contract contract;
    log.stream() << "Activating contract from test";
    contract.activate();

    QVERIFY2( UnitTestDB::isClosedDB(), "Database not closed after test" );
}

void ContractTest::insert() {

}



// http://localhost:8080/mytasks/issue102
/*
Skridt for at genskabe:

1) Åbn udlejning, vælg en lejer
2) Udlej tre genstande, id1, id2, id3
3) Tryk færdig
4) Åbn aflever, aflever genstand id1
5) Try afslut
6) Åbn udlejning, vælg en ny lejer
7) Udlej tre genstande, id1, id4, id5
8) Tryk færdig
9) Åbn aflever, aflever genstand id2
10) Tryk returner alle
11) Åbn aflever, aflever genstand id4
12) Tryk return alle => fejl, fordi genstand id1 ikke bliver afleveret, så
contractitem for den i kontrakt 2, ikke er OK.

Det kan jo heldigvis laves om til en unittest....
  */
void ContractTest::regression_returnAll() {
    Log::Logger log( "void ContractTest::regression_returnAll()" );
    UnitTestDB::resetDB( "ContractTest_regression_returnAll" );


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
    }

    // We need to reload the stuff from disk, because... stuff going on in contract1, has influenced stuff in contract2.

    {
        // Reload contract 1, and return all
        Contract contract1 = Contract::db_load( 1 );
        QCOMPARE( contract1.getState(), DB::Contract::State::active );
        contract1.returnAll();
        QCOMPARE( contract1.hasReturnableItems(), false );
    }
    {
        // Reload contract 1, and return all
        Contract contract2 = Contract::db_load( 2 );
        QCOMPARE( contract2.getState(), DB::Contract::State::active );
        contract2.returnAll();
        QCOMPARE( contract2.hasReturnableItems(), false );
    }

    QVERIFY2( false, "Incomplete unit test" );
}
