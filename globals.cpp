#include "globals.h"

// App
#include "log.h"
#include "exception.h"

Interceptor * Globals::interceptor;
PosPrinter * Globals::posPrinter;

using namespace Log;

namespace Globals  {
    PosPrinter & getPosPrinter() {
        Logger log("PosPrinter & getPosPrinter()");
        if ( ! posPrinter ) {
            throw Exception( Errors::InternalError )
                    << ( log.stream (error)
                         << "Internal error: global pointer to posprinter is not set.");
        }
        return *posPrinter;
    }


}; // namespace
