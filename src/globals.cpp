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

#include "globals.h"

// Qt
#include <QMessageBox>

// App
#include "log.h"
#include "exception.h"

// TODO: Make interface and stuff consistent...
Interceptor * Globals::interceptor;
Pos::Printer * Globals::posPrinter;
FeedbackObject * feedbackObject;


using namespace Log;

namespace Globals  {

    const QString expected_db_version = "49";

    FeedbackObject * getFeedbackObject() {
        Logger log("FeedbackObject * getFeedbackObject()");
        if ( ! feedbackObject ) {
            feedbackObject = new FeedbackObject();
        }
        return feedbackObject;
    }

    Pos::Printer & getPosPrinter() {
        Logger log("PosPrinter & getPosPrinter()");
        if ( ! posPrinter ) {
            throw Exception( Errors::InternalError )
                    << ( log.stream (error)
                         << "Internal error: global pointer to posprinter is not set.");
        }
        return *posPrinter;
    }

    bool checkPosPrinter() {
        Logger log("bool checkPosPrinter()");
        while( ! getPosPrinter().ready() ) {
            QMessageBox box( QMessageBox::Warning,
                             QObject::tr("Printer not ready"),
                             QObject::tr( "The receipt printer is not ready. Please check power and cables." ),
                             QMessageBox::Cancel | QMessageBox::Retry );
            box.setDetailedText( QObject::tr("1. Check the printer has power. The 'Power led' should be on.\n"
                                                "2. Check that all usb cables are properly connected.\n"
                                                "3. Turn the printer off\n"
                                                "4. Wait five seconds\n"
                                                "5. Then turn it back on\n"
                                                "6. Press the 'Retry' button to test the printer again") );
            int ret = box.exec();
            if ( ret != QMessageBox::Retry ) {
                return getPosPrinter().ready();
            }
        }
        return true;
    }
}; // namespace
