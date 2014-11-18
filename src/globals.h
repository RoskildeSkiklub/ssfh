/* * Snow Stuff For Hire - A POS-like system designed for very quick rental
 * operations at small ski resorts
 * Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
 *
 * This file is part of Snow Stuff For Hire.
 *
 * Snow Stuff For Hire is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Snow Stuff For Hire is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

/** \file
  * \brief Holds global declarations that needs to be accesible from everywhere in the system */

// Qt
#include <QString>

// app
#include "interceptor.h"
#include "posprinter.h"
#include "feedbackobject.h"

namespace Globals {

    /** \brief The database version expected */
    extern const QString expected_db_version;

    extern Interceptor * interceptor;
    // So it can be set up from main
    // TODO: Fix this, its broken.
    extern Pos::Printer * posPrinter;

    /** \brief Return reference to the global PosPrinter instance
      * \return A reference to the global PosPrinter instance */
    Pos::Printer & getPosPrinter();

    /** \brief Checks the global PosPrinter is available
      *
      * This checks the printer is connected/ready. If not, it displays a dialog box,
      * asking the user if they want to try again, or ignore */
    bool checkPosPrinter();

    /** \brief Return a pointer to the global (audio) feedback instance */
    FeedbackObject * getFeedbackObject();

    /** \brief Enum with commands from the barcode scanner */
    namespace BarcodeCommands {
        enum Code {
            OperationDone            = 0, // Used to signal done/close to rental/return windows
            OpenRentalWindow         = 1, // MainWindow - open rental window
            OpenReturnWindow         = 2, // MainWindow - open return window
            ContractRemoveIndividual = 3, // Command is parameterized with kind
            ContractAddIndividual    = 4, // Command is parameterized with kind
            ContractRemoveLiftCard   = 5, // Command is parameterized with kind
            ContractAddLiftCard      = 6, // Command is parameterized with kind.
        };

        class Command {
        public:
            Code m_code; // Code to match on for system
            QString m_label; // User text
            QString m_param; // Code param
            Command( const Code & code, const QString & label,
                     const QString & param = QString("") )
                : m_code( code ), m_label( label ), m_param( param )  {}
        };
    };

} // namespace;
#endif // GLOBALS_H
