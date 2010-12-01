#include "dksundhedskort.h"

// Qt
#include <QString>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>

// App
#include "log.h"
#include "exception.h"
#include "utility.h"
using namespace Log;


DKSundhedskort::DKSundhedskort( const QString & track1, const QString & track2 ) {
    Logger log( "DKSundhedskort::DKSundhedskort( const QString & track1, const QString & track2 )" );
    // Must match - get the cap value, convert to qulonglong
    QRegExp DKRE_track1( "(.{34})(.{34})\\d{3}(\\d{4})" );
    QRegExp DKRE_track2( "\\d{8}(\\d{10})\\d{19}" );

    if ( ! ( DKRE_track1.exactMatch( track1 )
        && DKRE_track2.exactMatch( track2 ) ) ) {
        log.stream( warn ) << "Missing match on at least one track, throwing";
        throw Exception( Errors::DKSundhedskortNoMatch )
                << ( log.stream( error ) << "Unable to match DKSundhedskort to tracks '"
                     << track1 << "' and/or '" << track2  << "'" );
    }
    QStringList surname_n_name( DKRE_track1.cap( 1 ).trimmed().split( "&" ) );
    if ( surname_n_name.size() != 2 ) {
        throw Exception( Errors::MagswipeNoMatch )
                << ( log.stream( error ) << "Wrong number of fields in name data: "
                     << surname_n_name.size() );
    }
    surname = capitalizeWords( fixUpDanishLetters( surname_n_name.at( 0 ) ) );
    name    = capitalizeWords( fixUpDanishLetters( surname_n_name.at( 1 ) ) );
    address = capitalizeWords( fixUpDanishLetters( DKRE_track1.cap( 2 ).trimmed() ) );
    zip     = DKRE_track1.cap( 3 ).trimmed();
    ssn     = DKRE_track2.cap( 1 ).trimmed();

    // Lookup city, ignore lookup errors
    city = "";
    try {
        QSqlQuery query;
        query_check_prepare( query, "select city from zipcity where zip = :zip" );
        query.bindValue( ":zip", zip );
        query_check_exec( query );
        query_check_first( query );
        city = query.value( 0 ).toString();
    }
    catch( ... ) {
        log.stream( warn ) << "Ignoring lookup error on zip '" << zip << "'";
    };

    // Country is always Danmark....
    country = "Danmark";

    log.stream() << "name: '" << name
            << "' surname: '" << surname
            << "', address: '" << address
            << "', zip: '" << zip
            << "', city: '" << city
            << "', country: '" << country
            << "', ssn: '" << ssn << "'";
}

QString DKSundhedskort::fixUpDanishLetters( const QString & input ) const {
    QString res = input;
    // FIXES WITHOUT caps-lock
    // danish ligatur AE becomes keycode 197, and is translated to c3a5, which is small a with oring
    // It needs to become capital AE, of course, which is c386
    res.replace( QString::fromUtf8( "\x0c3\x0a5" ), QString::fromUtf8( "\x0c3\x086" ) );
    // WITH caps-lock
    // AE => keycode 197, c385
    res.replace( QString::fromUtf8( "\x0c3\x085" ), QString::fromUtf8( "\x0c3\x086" ) );

    // Danish oe (o-dash) becomes keycode 39, translated into %27: ' / .
    // It needs to become OE, which is c398
    // It is the same both with and without caps lock.
    res.replace( QString::fromUtf8( "\x027" ), QString::fromUtf8( "\x0c3\x098" ) );

    return res;
}

