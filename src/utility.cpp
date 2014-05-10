/** \file
  * \brief Various utility methods */

// Qt
#include <QSqlQuery>
#include <QSqlError>
#include <QMap>
#include <QVariant>
#include <QString>
#include <QMessageBox>
#include <QStateMachine>
#include <QSet>
#include <QAbstractState>
#include <QSqlDatabase>
#include <QStringList>

// Application
#include "utility.h"
#include "log.h"
#include "exception.h"

using namespace Log;

void query_check_prepare( QSqlQuery & query, const QString & queryString ) {
    Logger log( "void query_check_prepare( const QSqlQuery & query, const QString & queryString )" );
    if ( ! query.prepare( queryString ) ) {
        throw Exception( Errors::DBError )
            << ( log.stream( error ) << "Error preparing query '"
                << query.lastQuery()
                << "' : '" << query.lastError().text() << "'" );
    }
    log.stream() << "Prepared query '" << query.lastQuery() << "'";


}

void query_check_exec( QSqlQuery & query ) {
    Logger log( "void query_check_exec( const QSqlQuery & query )" );
    log.stream() << "Preparing to execute query '" << query.lastQuery() << "'";
    // Dump any bound values
    QMapIterator<QString, QVariant> i(query.boundValues());
    if ( i.hasNext() ) {
        log.stream() << "Dumping bound values";
        while (i.hasNext()) {
            i.next();
            log.stream() << "Bound: '" << i.key().toAscii().data() << "': '"
                    << i.value().toString().toAscii().data() << "'";
        }
        log.stream() << "Done dumping bound values";
    }
    if ( ! query.exec() ) {
        throw Exception( Errors::DBError )
            << ( log.stream( error ) << "Error executing query '"
                << query.lastQuery()
                << "' : '" << query.lastError().text() << "'" );
    }
    log.stream() << "Executed query '" << query.executedQuery() << "'";
};

void query_check_first( QSqlQuery & query ) {
    Logger log( "void query_check_first()" );
    if ( ! query.first() ) {
        // Todo, fix this up.
        //! \todo Quick fix to get it out of log, change error => debug - we get this a lot when looking up hirers
        throw Exception( Errors::DBResultError )
            << ( log.stream( debug ) << "Error moving to first record of query '"
                 << query.lastQuery()
                 << "'' : '" << query.lastError().text() << "'" );
    }
    log.stream() << "Moved to first record of query '" << query.executedQuery() << "'";
}

void database_transaction(const QString &where) {
    Logger log("void database_transaction(const QString &where)");
    log.stream( debug ) << "Where is " << where;
    if ( !QSqlDatabase::database().transaction() ) {
        throw Exception( Errors::DBTransactionError )
                << ( log.stream( error )
                     << "Error while trying to start transaction: '"
                     << QSqlDatabase::database().lastError().text()
                     << "'. Called from '" << where << "'" );
    }
}

void database_commit(const QString &where) {
    Logger log("void database_commit(const QString &where)");
    log.stream( debug ) << "Where is " << where;
    if ( !QSqlDatabase::database().commit() ) {
        throw Exception( Errors::DBCommitError )
                << ( log.stream( error )
                     << "Error while trying to start commit: '"
                     << QSqlDatabase::database().lastError().text()
                     << "'. Called from '" << where << "'" );
    }
}

void database_rollback(const QString &where) {
    Logger log("void database_rollback(const QString &where)");
    log.stream( debug ) << "Where is " << where;
    if ( !QSqlDatabase::database().rollback() ) {
        throw Exception( Errors::DBRollbackError )
                << ( log.stream( error )
                     << "Error while trying to start rollback: '"
                     << QSqlDatabase::database().lastError().text()
                     << "'. Called from '" << where << "'" );
    }
}


void todofunc(const QString & where, const QString &msg) {
    Logger log( "void TODO(const QString &msg)" );
    log.stream( todo ) << "Todo message from '" << where << "' . Message is '" << msg << "'";
    QMessageBox::critical( NULL, "TODO MESSAGE",
                           QString( "<p>Got a TODO message</p><p><b>%0</b></p><p>From</p><p>%1</p>" )
                           .arg( msg ).arg( where ) );
}

QStringList get_current_states( const QStateMachine & stateMachine ) {
    Logger log("QStringList get_current_states( const QStateMachine & state_machine )");
    QSet<QAbstractState *> states = stateMachine.configuration();
    QAbstractState * statep;
    QStringList res;
    foreach( statep, states ) { // foreach OK here.
        log.stream() << "Adding state '" << statep->objectName() << "'";
        res << statep->objectName();
    }
    return res;
}

QString capitalizeWords(const QString &input) {
    QString res = input.toLower();
    bool prevWasWhiteSpace = true;
    for( int i = 0; i < input.size(); ++i ) {
        if ( prevWasWhiteSpace ) {
            res[i] = res.at( i ).toUpper();
        }
        prevWasWhiteSpace = res.at( i ).isSpace();
    }
    return res;
}

bool versionstring_qe( const QString &vera, const QString &verb ) {
    Logger log( "bool versionstring_qe( const QString &vera, const QString &verb )" );

    QStringList a = vera.split( "." );
    QStringList b = verb.split( "." );
    if ( a.size() != 3 && a.size() != 4 ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Version a passed to versionstring_qe not on expected form: '" << vera << "'" );
    }
    if ( b.size() != 3 && b.size() != 4 ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Version b passed to versionstring_qe not on expected form: '" << verb << "'" );
    }
    bool ok;
    int amajor = a[0].toInt( & ok );
    if ( !ok ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Version a passed to versionstring_qe not on expected form: '" << vera << "', a part was not a number." );
    }
    int aminor = a[1].toInt( & ok );
    if ( !ok ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Version a passed to versionstring_qe not on expected form: '" << vera << "', a part was not a number." );
    }
    int apatch = a[2].toInt( & ok );
    if ( !ok ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Version a passed to versionstring_qe not on expected form: '" << vera << "', a part was not a number." );
    }
    int bmajor = b[0].toInt( & ok );
    if ( !ok ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Version b passed to versionstring_qe not on expected form: '" << verb << "', a part was not a number." );
    }
    int bminor = b[1].toInt( & ok );
    if ( !ok ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Version b passed to versionstring_qe not on expected form: '" << verb << "', a part was not a number." );
    }
    int bpatch = b[2].toInt( & ok );
    if ( !ok ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Version b passed to versionstring_qe not on expected form: '" << verb << "', a part was not a number." );
    }

    return ( amajor > bmajor ) ||
            ( ( amajor == bmajor ) && ( ( aminor > bminor ) ||
                                       ( ( aminor == bminor)  && ( apatch >= bpatch)  ) ) );
}

QString database_get_config_value( const QSqlDatabase &db, const QString &key ) {
    Logger log( "QString database_get_config_value( const QSqlDatabase &db, const QString &key )" );
    if ( key.isEmpty() ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Empty key passed to database_get_config_value" );
    }
    QSqlQuery query( db );
    query_check_prepare( query, "select value from configuration where key=:key" );
    query.bindValue( ":key", key );
    query_check_exec( query );
    query_check_first( query );
    return query.value(0).toString();
}

void database_check_version( const QSqlDatabase &db, const QString &version ) {
    Logger log( "void database_check_version( const QDatabase &db, const QString &version )" );
    QString db_version = database_get_config_value( db, "db_version" );
    if ( db_version != version ) {
        throw Exception( Errors::DBVersionError,
                        QString( "Version mismatch. Expected: '%1', got : '%2'" )
                        .arg( version ).arg( db_version ),
                        db_version );
    }
}

void sqlite_check_setup( const QSqlDatabase &db ) {
    Logger log( "void sqlite_check_setup(const QSqlDatabase &db, const QString &version)" );
    QSqlQuery query( db );
    query_check_prepare( query, "select sqlite_version();" );
    query_check_exec( query );
    query_check_first( query );
    QString sqlite_version = query.value(0).toString();
    QString sqlite_min_version = database_get_config_value( db, "sqlite_min_version" );
    // Now compare
    if (! versionstring_qe( sqlite_version, sqlite_min_version ) ) {
        throw Exception( Errors::DBSqliteVersionError,
                        QString( "Version mismatch. Expected minimum: '%1', got : '%2'" )
                        .arg( sqlite_min_version ).arg( sqlite_version ),
                        sqlite_version );
    }
    // Enable pragma foreign_keys = on to ensure foreign key support
    query_check_prepare( query, "pragma foreign_keys = on;" );
    query_check_exec( query );
}


