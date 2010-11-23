#ifndef GLOBALS_H
#define GLOBALS_H

/** \file
  * \brief Holds global declarations that needs to be accesible from everywhere in the system */

// Qt
#include <QString>

#include "interceptor.h"
#include "posprinter.h"

namespace Globals {

    extern Interceptor * interceptor;
    extern Pos::Printer * posPrinter;

    /** \brief Return reference to the global PosPrinter instance
      * \return A reference to the global PosPrinter instance */
    Pos::Printer & getPosPrinter();

    /** \brief Checks the global PosPrinter is available
      *
      * This checks the printer is connected/ready. If not, it displays a dialog box,
      * asking the user if they want to try again, or ignore */
    bool checkPosPrinter();

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
