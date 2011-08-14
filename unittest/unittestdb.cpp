#include "unittestdb.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QSqlDatabase>
#include "log.h"
UnitTestDB::UnitTestDB()
{
}
void UnitTestDB::closeDB() {
    Log::Logger log( "void UnitTestDB::closeDB()" );
    if ( QSqlDatabase::database().isOpen() ) {
        log.stream() << "Database is open, closing it";
        QSqlDatabase::database().close();
    } else {
        log.stream() << "Database not open";
    }
}

bool UnitTestDB::resetDB() {
    Log::Logger log( "bool UnitTestDB::resetDB()" );
    // Also need to setup the database
    // May need to move this out to something else
    QString org_databasefile = "../schema/unittest.db";
    QString databasefile = "unittest.db";

    // If the database is already open, close it
    QSqlDatabase db;
    if ( QSqlDatabase::database().isOpen() ) {
        log.stream() << "Database is open, closing it";
        QSqlDatabase::database().close();
        db = QSqlDatabase::database();
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        if ( ! db.isValid() ) {
            log.stream( Log::fatal ) << "Unable to load QSQLITE database driver";
            //QMessageBox::critical( NULL, QObject::tr( "Unable to load QSQLITE database driver" ), QObject::tr( "Unable to load QSQLITE database driver. Aborting." )  );
            return false;
        }
    }
    // Copy from unittest directory
    QFile fromFile( org_databasefile );
    if( ! fromFile.exists() ) {
        log.stream( Log::fatal ) << "Input databasefile " << org_databasefile << " does not exist";
        return false;
    }
    QFile toFile( databasefile );
    if ( toFile.exists() ) {
        log.stream() << "Databasefile " << databasefile << " already exists, removing";
        if ( ! toFile.remove() ) {
            log.stream( Log::fatal ) << "Unable to remove databasefile " << databasefile;
            return false;
        }
    }
    if ( ! fromFile.copy( databasefile ) ) {
        log.stream( Log::fatal ) << "Unable to copy databasefile from " << org_databasefile
                                 << " to " << databasefile;
        return false;
    }


    log.stream() << "Setting up database";
    db.setHostName("localhost");
    db.setDatabaseName( databasefile );
    db.setUserName("");
    db.setPassword("");
    if ( ! db.open() ) {
        log.stream( Log::fatal ) << "Unable to open database '" << databasefile << "'";
        // QMessageBox::critical( NULL, QObject::tr( "Unable to open database" ), QString( QObject::tr( "Unable to open database specified as '%0'. Aborting." ) ).arg( databasefile ) );
        return false;
    }
    log.stream() << "Database successfully opened";
    return true;

}
