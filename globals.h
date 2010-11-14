#ifndef GLOBALS_H
#define GLOBALS_H

/** \file
  * \brief Holds global declarations that needs to be accesible from everywhere in the system */
#include "interceptor.h"
#include "posprinter.h"

namespace Globals {

    extern Interceptor * interceptor;
    extern Pos::Printer * posPrinter;

    Pos::Printer & getPosPrinter();

} // namespace;
#endif // GLOBALS_H
