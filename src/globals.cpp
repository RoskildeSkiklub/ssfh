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
