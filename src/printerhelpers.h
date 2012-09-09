#ifndef PRINTERHELPERS_H
#define PRINTERHELPERS_H

class QString;
class QStringList;

namespace PrinterHelpers {

    /** \brief Print a stringlist of e.g. item_ids as barcodes
      * \param header Optional header printed before the list of barcodes
      * \param barcodes The strings to print as barcodes
      * \param footer Optional footer printer after the list of barcodes
      * \returns true if printing was succesfull, false otherwise
      *
      * Note, empty lines in the stringlist is ignored. */
    bool printStringsAsBarcodes( const QString & header,
                             const QStringList & barcodes,
                             const QString & footer );
}

#endif // PRINTERHELPERS_H
