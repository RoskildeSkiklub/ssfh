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
    bool printItemsSplitted( const QString & header1, const QString & header2,
                             QSqlQuery & query, bool includeBarCodes,
                             int sortLevel, int maxItemsPrPage );

}

#endif // PRINTERHELPERS_H
