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

// (surname&name)(address)...(zip)...(ssn)
QRegExp DKSundhedskort::regexp( QString::fromUtf8( "%(.{34})(.{34})\\d{3}(\\d{4})_\x0c3\x086\\d{8}(\\d{10})\\d{19}_\r" ) );

DKSundhedskort::DKSundhedskort( const QRegExp & regexp ) {
    Logger log( "DKSundhedskort::DKSundhedskort( const QRegExp & regexp )" );
    // Must match - get the cap value, convert to qulonglong
    if ( 4 != regexp.captureCount() ) {
        throw Exception( Errors::MagswipeNoMatch )
                << ( log.stream( error ) << "Unknown magswipe: Wrong number of captures in regexp: "
                    << regexp.captureCount() );
    }
    QStringList surname_n_name( regexp.cap( 1 ).trimmed().split( "&" ) );
    if ( surname_n_name.size() != 2 ) {
        throw Exception( Errors::MagswipeNoMatch )
                << ( log.stream( error ) << "Wrong number of fields in name data: "
                     << surname_n_name.size() );
    }
    surname = capitalizeWords( fixUpDanishLetters( surname_n_name.at( 0 ) ) );
    name    = capitalizeWords( fixUpDanishLetters( surname_n_name.at( 1 ) ) );
    address = capitalizeWords( fixUpDanishLetters( regexp.cap( 2 ).trimmed() ) );
    zip     = regexp.cap( 3 ).trimmed();
    ssn     = regexp.cap( 4 ).trimmed();

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

    log.stream() << "name: '" << name
            << "' surname: '" << surname
            << "', address: '" << address
            << "', zip: '" << zip
            << "', city: '" << city
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

QString DKSundhedskort::capitalizeWords(const QString &input) const {
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
