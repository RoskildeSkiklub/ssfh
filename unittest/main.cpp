#include "AutoTest.h"
#include <QDebug>
#include <QSqlDatabase>


// Lots of the classes require the log module to be initialized
#include "log.h"

// And, we need some database control
#include "unittestdb.h"

#if 0

// This is all you need to run all the tests
TEST_MAIN
#else
// Or supply your own main function
int main(int argc, char *argv[])
{
    // Need to set up logging...
    Log::Log::init( "unittest.log" );
    Log::Logger log( "int main(int argc, char *argv[])" );

    // Check that the unittest db setup stuff works.
    if ( ! UnitTestDB::resetDB() ) {
        return -42;
    }



    // Actually run the tests
    int failures = AutoTest::run(argc, argv);
    if (failures == 0)
    {
	qDebug() << "ALL TESTS PASSED";
    }
    else
    {
	qDebug() << failures << " TESTS FAILED!";
    }
    return failures;
}
#endif
