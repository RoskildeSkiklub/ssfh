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

void database_check_version( const QSqlDatabase &db, const QString &version ) {
    Logger log( "void database_check_version( const QDatabase &db, const QString &version )" );
    QSqlQuery query;
    query_check_prepare( query, "select value from configuration where key='db_version'" );
    query_check_exec( query );
    query_check_first( query );
    QString db_version = query.value(0).toString();
    if ( db_version != version ) {
        throw Exception( Errors::DBVersionError, "", db_version );
    }
}



