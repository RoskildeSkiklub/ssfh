#ifndef GLOBALS_H
#define GLOBALS_H

/** \file
  * \brief Holds global declarations that needs to be accesible from everywhere in the system */
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

} // namespace;
#endif // GLOBALS_H
