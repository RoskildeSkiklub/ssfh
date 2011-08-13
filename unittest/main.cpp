#include "AutoTest.h"
#include <QDebug>

// Lots of the classes require the log module to be initialized
#include "log.h"

#if 0

// This is all you need to run all the tests
TEST_MAIN
#else
// Or supply your own main function
int main(int argc, char *argv[])
{
    // Need to set up logging...
    Log::Log::init( "unittest.log" );

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
