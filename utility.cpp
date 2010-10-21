/** \file
  * \brief Various utility methods */

// Qt
#include <QSqlQuery>
#include <QSqlError>
#include <QMap>
#include <QVariant>
#include <QString>

// Application
#include "utility.h"
#include "log.h"
#include "exception.h"

using namespace Log;

void query_check_prepare( QSqlQuery & query, const QString & queryString ) {
    Logger log( "void query_check_prepare( const QSqlQuery & query )" );
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
    if ( ! query.exec() ) {
        throw Exception( Errors::DBError )
            << ( log.stream( error ) << "Error executing query '"
                << query.lastQuery()
                << "' : '" << query.lastError().text() << "'" );
    }
    log.stream() << "Executed query '" << query.executedQuery() << "'";
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
};

void query_check_first( QSqlQuery & query ) {
    Logger log( "void query_check_first()" );
    if ( ! query.first() ) {
        // Todo, fix this up.
        throw Exception( Errors::DBResultError )
            << ( log.stream( error ) << "Error moving to first record of query '"
                 << query.lastQuery()
                 << "'' : '" << query.lastError().text() << "'" );
    }
    log.stream() << "Moved to first record of query '" << query.executedQuery() << "'";
}

