#include "printerhelpers.h"

// Some helper functions for printing

#include <QString>
#include <QStringList>
#include <QVector>
#include <QVariant>
#include <QSqlQuery>

// app
#include "log.h"
#include "globals.h"
#include "posprinter.h"
#include "utility.h"
#include "item.h"

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


    bool printItemsSplitted( const QString & header1, const QString & header2,
                             QSqlQuery & query, bool includeBarCodes,
                             int sortLevel, int maxItemsPrPage ) {
        Logger log("void doItemSplitPrint( const QString & header1, const QString & header2, const QSqlQuery & query, bool includeBarCodes, int sortLevel, int maxItemsPrPage )");
        query_check_exec( query );
        if ( ! query.first() ) {
            log.stream() << "Query did not find anything to print";
            return false;
        } else {
            if ( ! Globals::checkPosPrinter() ) {
                log.stream() << "No printer available";
                return false;
            } else {
                // Track count here. This is reset for each page/receipt
                int countItemsOnCurrentReceipt = 0;
                // Track the content of the last printed item. at(0) is also reset for each page
                QVector<QVariant> prevItem( 1 + sortLevel );

                Pos::Printer & posp( Globals::getPosPrinter() );
                posp.setFontSize( 1, 2 );

                bool needToStartReceipt = true; // Tracks if we need to start a receipt
                bool shouldEndReceipt = false;  // Tracks if we are in a receipt, and need to end it when done

                log.stream( info ) << "Starting print of check list";
                do {
                    // Check if we should do a pagebreak, *before* printing the next value now.
                    bool breakPage = false;
                    // Check for break because of sort split
                    if ( prevItem.at(0).isValid() ) {
                        for( int i = 1; i <= sortLevel; ++i ) {
                            if ( prevItem.at( i ) != query.value( i ) ) {
                                breakPage = true;
                                break;
                            }
                        }
                    }
                    // Adjust break for max items pr. receipt reached.
                    breakPage = breakPage
                                || ( maxItemsPrPage != 0 && countItemsOnCurrentReceipt != 0
                                     && countItemsOnCurrentReceipt % maxItemsPrPage == 0 );

                    // Test for breakpage needed
                    if ( breakPage ) {
                        posp.endReceipt();
                        needToStartReceipt = true;
                        shouldEndReceipt = false;
                    }

                    // Start new receipt, if needed, set up for final closure.
                    if ( needToStartReceipt ) {
                        posp.startReceipt();
                        needToStartReceipt = false;
                        countItemsOnCurrentReceipt = 0;
                        prevItem[ 0 ] = QVariant();
                        if ( ! header1.isEmpty() ) {
                            posp << header1 << Pos::endl;
                        }
                        if ( ! header2.isEmpty() ) {
                            posp << header2 << Pos::endl;
                        }
                        posp << Pos::hr;
                        shouldEndReceipt = true;
                    }

                    // Print items.
                    // posp << Pos::endl;
                    QString item_id = query.value(0).toString();
                    Item item( Item::db_load( item_id ) );
                    posp << item.toReceiptString() << Pos::endl;
                    if ( includeBarCodes ) {
                        posp.setFontSize();
                        posp << Pos::center << Pos::Barcode( item_id ) << Pos::hr;
                        posp.setFontSize(1, 2);
                    }

                    // Adjust split controllers
                    ++countItemsOnCurrentReceipt;
                    for( int i = 0; i <= sortLevel; ++i ) {
                        prevItem[ i ] = query.value( i );
                    }

                } while( query.next() ); // while records in the query

                if ( shouldEndReceipt ) {
                    posp.endReceipt();
                }
                posp.setFontSize();
                return true;
            } // Printer available
        } // Records available
    } // end doItemSplit
}
