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
}

QString Hirer::toHtml() const {
    QString res = QString( "<li>%6<em>%1 %2</em><br/>%3<br/>%4  %5</li>" )
                  .arg( m_firstName ).arg( m_lastName ).arg( m_streetAddress ).arg( m_zip ).arg( m_city );
    if ( !m_memberId.isEmpty() ) {
        res = res.arg( tr( "MemberId: " ) + m_memberId + "<br/>" );
    } else {
        res = res.arg("");
    }
    return res;
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

    m_valid         = true;
}
