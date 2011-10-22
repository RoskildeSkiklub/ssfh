#include "AutoTest.h"
#include <QDebug>
#include <QSqlDatabase>
#include <iostream>


// Lots of the classes require the log module to be initialized
#include "log.h"

// And, we need some database control
#include "unittestdb.h"

using namespace std;
#if 0

// This is all you need to run all the tests
TEST_MAIN
#else
// Or supply your own main function
int main(int argc, char *argv[])
{
    try {
        // Need to set up logging...
        Log::Log::init( "unittest.log" );
        Log::Logger log( "int main(int argc, char *argv[])" );

        // Check that the unittest db setup stuff works.
        if ( ! UnitTestDB::resetDB("main") ) {
            cerr << "main.cpp:29: error: Unittest: Unable to reset db, check unittest.log" << endl;
            return -42;
        }
        // And, all tests that needs the database, must mark it so
        UnitTestDB::closeDB();

        // Actually run the tests
        int failures = AutoTest::run(argc, argv);
        if (failures == 0)
        {
            cout << "ALL TESTS PASSED" << endl;
        }
        else
        {
            cout << failures << " TESTS FAILED!" << endl;
        }
        return failures;
    }
    catch ( ... ) {
        cerr << "main.cpp:48: error: Unittest: Some kind of exception was thrown. Check unittest.log" << endl;
        return -42;
    }
}
#endif
