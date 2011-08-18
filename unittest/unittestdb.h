#ifndef UNITTESTDB_H
#define UNITTESTDB_H

#include <QString>

class UnitTestDB
{
public:
    UnitTestDB();

    /**
      * Resets the test database
      *
      * \param testId An id, used when wishing to keep the databases. Should be filename friendly */
    static bool resetDB( const QString & testId );

    /**
      * Makes sure the database is closed */
    static void closeDB();

    /**
      * Query the open/close state of the database connection
      * \return true if closed, false otherwise */
    static bool isClosedDB();

};

#endif // UNITTESTDB_H
