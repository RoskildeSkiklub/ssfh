#ifndef UNITTESTDB_H
#define UNITTESTDB_H

class UnitTestDB
{
public:
    UnitTestDB();

    /**
      * Resets the test database */
    static bool resetDB();
};

#endif // UNITTESTDB_H
