//
// Snow Stuff For Hire - A POS-like system designed for very quick rental
// operations at small ski resorts
// Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
//
// This file is part of Snow Stuff For Hire.
//
// Snow Stuff For Hire is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Snow Stuff For Hire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
//

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
