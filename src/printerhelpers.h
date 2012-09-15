#ifndef PRINTERHELPERS_H
#define PRINTERHELPERS_H

class QString;
class QStringList;
class QSqlQuery;

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

    /** \brief Method to print a list of items, and split on certain criterias
      * \param header1 A header to be printed before the actual list
      * \param header2 Another header to be printed before the actual list
      * \param query A query ready to exec, that return a list of item ids
      * \param includeBarCodes Wheter to print barcodes or not
      * \param sortLevel Reflects the number of criterias in the "order by" part of the query
      * \param maxItemsPrPage The number of items that can be printed before splitting the page
      * \returns true if printing was succesfull, false otherwise */
    bool doItemSplitPrint( const QString & header1, const QString & header2,
                           QSqlQuery & query, bool includeBarCodes,
                           int sortLevel, int maxItemsPrPage );

}

#endif // PRINTERHELPERS_H
