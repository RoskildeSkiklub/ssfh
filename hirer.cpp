// qt
#include <QVariant>

// App
#include "hirer.h"
#include "exception.h"
#include "log.h"
#include "utility.h"

using namespace Log;

Hirer::Hirer() : m_valid( false )
{
}

Hirer::Hirer( qlonglong id ) : m_valid( false ){
    Logger log( "Hirer::Hirer( qlonglong id )" );
    log.stream() << "Loading hirer with id: " << id;
    QSqlQuery query;
    query_check_prepare( query,
                         "select id, memberId, ssn, firstName, lastName, streetAddress, zip, city, country, note "
                         "from hirers "
                         "where id = :id" );
    query.bindValue( ":id", id );
    query_check_exec( query );
    query_check_first( query );
    Hirer tmp( query );
    // Probably a violation of some rule or other
    *this = tmp;
    log.stream() << "Hirer loaded and created from database: " << toString();
}

Hirer::Hirer( const QString & firstName, const QString & lastName,
              const QString & streetAddress, const QString & zip,
              const QString & city, const QString & country, const QString & ssn )
                  : m_memberId( "" ), m_ssn( ssn ), m_firstName( firstName ), m_lastName( lastName ),
                  m_streetAddress( streetAddress ), m_zip( zip ),
                  m_city( city ), m_country( country ), m_note( "" ) {
    Logger log( "Hirer::Hirer( const QString & firstName, const QString & lastName, const QString & streetAddress, const QString & zip, const QString & city, const QString & ssn )" );
    log.stream() << "TODO: Lookup/Create stuff in database.";
    // First, exact match on everything...
    try {
        QSqlQuery query;
        query_check_prepare( query, "select id, memberId, ssn, firstName, lastName, streetAddress, zip, city, country, note "
                             "from hirers "
                             "where firstname = :firstName "
                             "and lastName = :lastName "
                             "and streetAddress = :streetAddress "
                             "and zip = :zip "
                             "and city = :city "
                             "and country = :country "
                             "and ssn = :ssn" );
        query.bindValue( ":firstName", firstName );
        query.bindValue( ":lastName", lastName );
        query.bindValue( ":streetAddress", streetAddress );
        query.bindValue( ":zip", zip );
        query.bindValue( ":city", city );
        query.bindValue( ":country", country );
        query.bindValue( ":ssn", ssn );
        query_check_exec( query );
        query_check_first( query );
        // lazy
        Hirer tmp( query );
        if ( query.next() ) {
            log.stream( warn ) << "More than one match on hirer";
        }
        // Probably a violation of some rule or other
        *this = tmp;
        return;
    }
    catch( ... ) {
        log.stream() << "Failed to locate hirer based on all information exact match. Trying without ssn";
    }

    try {
        QSqlQuery query;
        query_check_prepare( query, "select id, memberId, ssn, firstName, lastName, streetAddress, zip, city, country, note "
                             "from hirers "
                             "where firstname = :firstName "
                             "and lastName = :lastName "
                             "and streetAddress = :streetAddress "
                             "and zip = :zip "
                             "and city = :city "
                             "and country = :country "
                             "and ssn = ''" );
        query.bindValue( ":firstName", firstName );
        query.bindValue( ":lastName", lastName );
        query.bindValue( ":streetAddress", streetAddress );
        query.bindValue( ":zip", zip );
        query.bindValue( ":city", city );
        query.bindValue( ":country", country );
        query_check_exec( query );
        query_check_first( query );
        // lazy
        Hirer tmp( query );
        if ( query.next() ) {
            log.stream( warn ) << "More than one match on hirer";
        }
        // Probably a violation of some rule or other
        *this = tmp;
        return;
    }
    catch( ... ) {
        log.stream() << "Failed to locate hirer based on all information exact match with empty ssn. Will create hirer.";
    }

    // Create and insert into the database
    QSqlQuery query;
    query_check_prepare( query, "insert into hirers ( memberId, ssn, firstName, lastName, streetAddress, zip, city, country, note ) "
                         "values( :memberId, :ssn, :firstName, :lastName, :streetAddress, :zip, :city, :country, :note )" );
    query.bindValue( ":memberId", m_memberId );
    query.bindValue( ":firstName", firstName );
    query.bindValue( ":lastName", lastName );
    query.bindValue( ":streetAddress", streetAddress );
    query.bindValue( ":zip", zip );
    query.bindValue( ":city", city );
    query.bindValue( ":country", country );
    query.bindValue( ":ssn", ssn );
    query.bindValue( ":note", m_note );
    query_check_exec( query );
    // Read back the last value
    query_check_prepare( query, "select last_insert_rowid() from hirers" );
    query_check_exec( query );
    query_check_first( query );
    m_id = query.value( 0 ).toLongLong();
    log.stream( info ) << "Created hirer with id '" << m_id << "'";
    m_valid = true;
}

QString Hirer::toHtml() const {
    QString res = QString( "%6<em>%1 %2</em><br/>%3<br/>%4  %5" )
                  .arg( m_firstName ).arg( m_lastName ).arg( m_streetAddress ).arg( m_zip ).arg( m_city );
    if ( !m_memberId.isEmpty() ) {
        res = res.arg( tr( "MemberId: " ) + m_memberId + "<br/>" );
    } else {
        res = res.arg("");
    }
    return res;
}


QString Hirer::toString() const {
    Logger log("QString Hirer::toString()");
    return QString( "id: '%0', memberId: '%1', ssn: '%2', firstName: '%3', "
                    "lastName: '%4'', streetAddress: '%5', zip: '%6', city: '%7', country: '%8', "
                    "note: '%9'" )
            .arg( m_id ).arg( m_memberId ).arg( m_ssn ).arg( m_firstName ).arg( m_lastName )
            .arg( m_streetAddress ).arg( m_zip ).arg( m_city ).arg( m_country )
            .arg( m_note );
}

Hirer::Hirer( const QSqlQuery & query ) : m_valid( false ) {
    Logger log( "Hirer::Hirer( const QSqlQuery & query )" );
    if ( ! query.isActive() ) {
        throw Exception( Errors::DBQueryInActive )
                << ( log.stream( error )
                     << Hirer::tr("Query passed to function was not active") );
    };
    m_id            = query.value( 0 ).toLongLong();
    m_memberId      = query.value( 1 ).toString();
    m_ssn           = query.value( 2 ).toString();
    m_firstName     = query.value( 3 ).toString();
    m_lastName      = query.value( 4 ).toString();
    m_streetAddress = query.value( 5 ).toString();
    m_zip           = query.value( 6 ).toString();
    m_city          = query.value( 7 ).toString();
    m_country       = query.value( 8 ).toString();
    m_note          = query.value( 9 ).toString();
    log.stream( info ) << "Loaded hirer from database, with id: '" << m_id << "'";
    m_valid         = true;
}
