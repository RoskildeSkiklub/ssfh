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

#include "exception.h"

// qt
#include <QMessageBox>

QString Exception::toString() const {
    return QString( tr( "Exception: %1, '%2'" ) ).arg( m_statusCode ).arg( m_description );
}

void default_critical( Log::Logger & log, const QString & msg ) {
    log.stream( Log::error ) << "Caught exception, showing default_critical MessageBox to user. Exception msg is : '"
                << msg << "'";
    QMessageBox::critical( 0, Exception::tr("Unexcepted error"),
                           Exception::tr("<p><b>An unexpected error happened</b></p>"
                                    "<p>Please try one of the following workarounds:</p>"
                                    "<p><ul><li>Retry the operation</li>"
                                    "<li>Close, then reopen the dialog and try again</li>"
                                    "<li>Restart the application and try again</li></ul></p>"
                                    "<p>The error message was:<br><b>%1</b></p>"
                                    "<p>More information may be available in the log</p>")
                           .arg( msg ) );
}
