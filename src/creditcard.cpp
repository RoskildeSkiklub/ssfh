#include "creditcard.h"

// Qt
#include <QString>
#include <QRegExp>

// app
#include "exception.h"
#include "log.h"
#include "utility.h"

using namespace Log;

CreditCard::CreditCard(const QString &track1, const QString &track2 )  {
    Logger log("CreditCard::CreditCard(const QString &track1, const QString &track)");
    // We actually only check track1...
    QRegExp track1_re( "B(\\d{16})&([^-]+)-([^&]+)&.*" );
    if ( ! track1_re.exactMatch( track1 ) ) {
        log.stream( warn ) << "Missing match on at least one track, throwing";
        throw Exception( Errors::CreditCardNoMatch )
                << ( log.stream( error ) << "Unable to match CreditCard to tracks '"
                     << track1 << "' and/or '" << track2  << "'" );
    }
    surname = capitalizeWords( track1_re.cap( 2 ).trimmed() );
    name = capitalizeWords( track1_re.cap( 3 ).trimmed() );
    cardnumber = track1_re.cap( 1 );
    log.stream() << "name: '" << name
            << "' surname: '" << surname
            << "' cardnumber: '" << cardnumber << "'";
}
