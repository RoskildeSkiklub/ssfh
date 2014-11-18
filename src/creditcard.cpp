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
                << ( log.stream( warn ) << "Unable to match CreditCard to tracks '"
                     << track1 << "' and/or '" << track2  << "'" );
    }
    surname = capitalizeWords( track1_re.cap( 2 ).trimmed() );
    name = capitalizeWords( track1_re.cap( 3 ).trimmed() );
    cardnumber = track1_re.cap( 1 );
    log.stream() << "name: '" << name
            << "' surname: '" << surname
            << "' cardnumber: '" << cardnumber << "'";
}
