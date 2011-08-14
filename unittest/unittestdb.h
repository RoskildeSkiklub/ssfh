#ifndef UNITTESTDB_H
#define UNITTESTDB_H

class UnitTestDB
{
public:
    UnitTestDB();

    /**
      * Resets the test database */
    static bool resetDB();

    /**
      * Makes sure the database is closed */
    static void closeDB();

};

#endif // UNITTESTDB_H
