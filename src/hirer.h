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

#ifndef HIRER_H
#define HIRER_H

// qt
#include <QString>
#include <QSqlQuery>
#include <QCoreApplication>

// In memory representation of a hirer

class Hirer {
    Q_DECLARE_TR_FUNCTIONS(Hirer)
public:

    qlonglong m_id; // 64 bit signed
    QString m_memberId;
    QString m_ssn;
    QString m_firstName;
    QString m_lastName;
    QString m_streetAddress;
    QString m_zip;
    QString m_city;
    QString m_country;
    QString m_note;


    /** \brief Create a hirer instance from information in the database
     * \param id The id of the hirer in the database
     * \return A Hirer instance with the values loaded from the database
     *
     * Note, the methods throw, if the hirer is not present in the database! */
    Hirer( qlonglong id );

    /** \brief Lookup or create a new hirer instance, based on values
      * \param firstName The firstName of the hirer
      * \param lastName The lastName of the hirer
      * \param streetAddress The streetAddress of the hirer
      * \param zip The zipcode of the hirer
      * \param city The city of the hirer
      * \param country The country of the hirer
      * \param ssn The SSN of the hirer
      *
      * A lookup in the database is performed. If a hirer is found, with *exact* matches
      * on all values, the *first* of these hirers is used.
      * If the first lookup fails, another lookup is performed, where SSN is set to the empty string.
      * Again, if exact matches are found, the first is used.
      * Ideally there should be only one match. A warning is logged if there are more matches.
      * If no hirer is found, a hirer is created, based on these values. */
    Hirer( const QString & firstName, const QString & lastName,
           const QString & streetAddress, const QString & zip,
           const QString & city, const QString & country, const QString & ssn );

    Hirer();

    /** \brief Check if this hirer is valid, that is, contains valid data
      * \return True if valid, false otherwise */
    bool isValid() const { return m_valid; };

    /** \brief Format as HTML for use in dialogs */
    QString toHtml() const;

    /** \brief Insert this hirer into the database
      *
      * Use the embedded values in this Hirer instance - expect an
      * id - to create a new hirer. Load the id back in to memory,
      * and set the hirer to valid. */
     void db_insert();

     /** \brief Update this hirer in the database
       *
       * Use the embedded values in this Hirer instance - expect an
       * id - to update the hirer in the database. Requires that
       * the hirer is valid. */
      void db_update();

private:
    /** \brief Holds the valid state */
    bool m_valid;

    /** \brief Create from an QSqlQuery
      * \param query The executed QSqlQuery
      *
      * This method creates a Hirer instance from the values in the QSqlQuery.
      * The query must be active and the orders of the colums must match the assumptions in the code. */
    Hirer( const QSqlQuery & query );

    /** \brief String representation for log files */
    QString toString() const;



};

#endif // HIRER_H
