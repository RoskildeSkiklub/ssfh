#include "contracttest.h"

#include "contract.h"
#include "unittestdb.h"
#include "db_consts.h"
#include "exception.h"

#include "log.h"

#include "unittest_utilities.h"

// Default should not do anything in the database.
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

// Test various methods on setting/getting a hirer
void ContractTest::hirerManip1() {
    Log::Logger log( "void ContractTest::hirerManip1" );
    // Set with invalid
    {
        UnitTestDB::closeDB();
        Contract contract;
        QCOMPARE( contract.getHirer().isValid(), false );
        Hirer hirer;
        QCOMPARE( hirer.isValid(), false );
        QVERIFY_THROW( contract.setHirer( hirer ), Exception );
    }
    // Set with valid
    {
        UnitTestDB::resetDB( "ContractTest_hirerManip1_1" );
        Contract contract;
        QCOMPARE( contract.getHirer().isValid(), false );
        Hirer hirer("joe", "doe", "someaddline", "zip", "city", "country", "ssn" );
        QCOMPARE( hirer.isValid(), true );
        contract.setHirer(hirer);
        QCOMPARE( contract.getHirer().isValid(), true );
        QCOMPARE( contract.getHirer().m_firstName, QString( "joe" ) );
        QCOMPARE( contract.getId(), -1LL );
        UnitTestDB::closeDB();
       //  QCOMPARE( hirer, contract.getHirer() );
    }
    // TODO: More tests could be added here, such as setting in an invalid state. Set in invalid state

}

// And some on duration stuff
void ContractTest::durationManip1() {
    Log::Logger log( "void ContractTest::durationManip1()" );
    UnitTestDB::closeDB();
    Contract contract;
    // TODO : Test duration stuff. Eventually.
}

void ContractTest::activate() {
    Log::Logger log( "void ContractTest::activate()" );

    // Unable to activate contract with invalid/no hirer and no items
    {
        UnitTestDB::resetDB( "ContractTest_activate_0" );
        Contract contract;
        log.stream() << "Activating contract with no hirer or items";
        QSKIP("THIS TEST IS CURRENTLY DISABLED", SkipAll);
        QVERIFY_THROW( contract.activate(), Exception );
        UnitTestDB::closeDB();
    }
    // Unable to activate contract with hirer but no items
    {
        UnitTestDB::resetDB( "ContractTest_activate_1" );
        Contract contract;
        Hirer hirer("joe", "doe", "someaddline", "zip", "city", "country", "ssn" );
        QCOMPARE( hirer.isValid(), true );
        contract.setHirer( hirer );
        log.stream() << "Activating contract with hirer but no items";
        QVERIFY_THROW( contract.activate(), Exception );
        UnitTestDB::closeDB();
    }
    // TODO: Actually be able to activate one with hirer and items.
}


void ContractTest::insert() {

}



// http://localhost:8080/mytasks/issue102
/*
  A bug in the code was introduced with the returnAll method/operation.
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
        contract2.addItem( "1" ); // **
        contract2.addItem( "4" );
        contract2.addItem( "5" );
        contract2.activate();
        QCOMPARE( contract2.getState(), DB::Contract::State::active );
        QCOMPARE( contract2.getId(), (long long) 2 );
    }

    // We need to reload the contracts from the db, because
    // they influence eachother, and only "talks" to eachother through the
    // db. First time is marked with **. If contract1 was used without reloading
    // item 1 in it, would still be marked out, and we would not be
    // able to provoke the error. Likewise with the returnAll on contract2.

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
        QSKIP("THIS TEST IS CURRENTLY DISABLED", SkipAll);
        QCOMPARE( contract2.hasReturnableItems(), false );
    }

    QVERIFY2( false, "Just to make sure it keeps failing - remove when check is OK." );
}

