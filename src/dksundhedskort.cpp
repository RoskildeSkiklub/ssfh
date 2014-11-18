//
// Snow Stuff For Hire - A POS-like system designed for very quick rental
// operations at small ski resorts
// Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
//
// This file is part of Snow Stuff For Hire.
//
// Snow Stuff For Hire is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Snow Stuff For Hire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
//

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
                << ( log.stream( warn ) << "Unable to match DKSundhedskort to tracks '"
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

    // I have never actually seen an aring, but I think it is ¨, that is d168/0xA8
    res.replace( QString::fromUtf8( "\x0a8" ), QString::fromUtf8( "\x0c3\x085" ) );

    return res;
}

