#include "printerhelpers.h"

// Some helper functions for printing

#include <QString>
#include <QStringList>

// app
#include "log.h"
#include "globals.h"
#include "posprinter.h"

using namespace Log;

namespace PrinterHelpers {

    bool printStringsAsBarcodes( const QString & header,
                             const QStringList & barcodes,
                             const QString & footer ) {
        Logger log("bool printStringsAsBarcodes( const QString & header, const QStringList & barcodes, const QString & footer )");
        if ( Globals::checkPosPrinter() ) {
            log.stream() << "Starting to print item ids";
            Pos::Printer & posp( Globals::getPosPrinter() );
            posp.setFontSize();
            posp.startReceipt();
            if ( !header.isEmpty() ) {
                posp << header << Pos::hr;
            }
            QString line;
            foreach ( line, barcodes ) {
                // Ignore empty lines
                if ( ! line.isEmpty() ) {
                    posp << Pos::center << Pos::Barcode( line ) << Pos::hr; }
            }
            if ( !footer.isEmpty() ) {
                posp << footer << Pos::endl;
            }
            posp.endReceipt();
            return true;
        } else {
            return false;
        }
    } // printStringsAsBarcodes

}
