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

#ifndef ITEM_H
#define ITEM_H

// Qt
#include <QObject>
#include <QCoreApplication>
#include <QString>

// In memory representation of an item

class Item {
    Q_DECLARE_TR_FUNCTIONS(Item);

public:
    /** \brief Construct from values, sets state to "in" */
    Item( QString id, QString type, QString size, QString mark, QString model, QString year,
          QString condition, long price, QString rentalgroup, QString note );

    /** \brief Lookup an item, and book it, if free.
      * \param id of item to lookup and book
      * \param contract_id Id of booking contract, used for logging
      * \return An in memory representation of an item. Booked in the database.
      *
      * This loads the item and books it.
      * If not available for booking, it throws (ItemUnavailable).
      * The contractId is used for logging, nothing else. */
    static Item locate_and_book_in_db( const QString & id, qlonglong contract_id );

    //! \todo Document
    /** \brief Locate and load an item from the database
      * \param id The id of the item to load
      *
      * If the item is not known in the database, it throws (ItemDoesNotExist) */
    static Item db_load( const QString & id );

    /** \brief Insert this item into the database.
      * Will except on duplicate */
    void db_insert();

    /** \brief Return HTML suitable for the rental dialog
      * \return HTML suitable for the rental dialog */
    QString toRentalHtml() const;

    /** \brief Return HTML representation of item
      * \return HTML representation of item */
    QString toHtml() const;

    /** \brief Return string suitable for a receipt
      * \return String suitable for receipt */
    QString toReceiptString() const;

    /** \brief Default constructor */
    Item();

    /** \brief Return the id of the item
      * \return The id of the item */
    QString getId() const;

    /** \brief Return the type of the item
      * \return The type of the item */
    QString getType() const;

    /** \brief Return the rental group
      * \return The rental group of the item */
    QString getRentalGroup() const;

    /** \brief Get the state of the item
      * \return The state of the item */
    QString getState() const;

    /** \brief Update the state of this item in the database to "out"
      * \param reason The reason for the setting, provided by client, and loggede to the db.
      *
      * This updates the state in the database, and also updates the in-memory state
      * The state may not already be out */
    void db_setToOut( const QString & reason );

    /** \brief Update the state of this item in the database to "in"
      *
      * This updates the state in the database, and also updates the in-memory state */
    void db_setToIn( const QString & reason );

    /** \brief Update only the state of the item.
      * \param state The new state
      * \param reason The reason for the state change.
      *
      * This method updates only the state in the database, and in
      * the in memory representation.
      * It will throw, if the state is not known or illegal for this method.
      * 'out' and 'booked' are illegal for this method. */
    void db_forceState( const QString & state, const QString & reason );

    /** \brief Give an item a new id
      * \param from_id The id of the item to give a new id
      * \param to_id The id that the item shall have
      * \throws If any kind of error.
      *
      * This method is used to "relabel" stuff. Labels fall of, and items
      * need new labels. This method makes sure that the history is "moved" in
      * the database as well, and that a suitable item event is logged in the database.
      * from_id must exists in the database, to_id must not.
      * This method should not be called, if from_id is loaded into memory
      * somewhere else in the system. That would be bad.
      * It should work with items that are currently out, contracts that are parked,
      * etc. (But not stuff that is loaded into memory).
      *
      * \note It is very important that no other query is active, when you call
      * this method, as it will fail because of locking issues very easily. */
    static void db_reid( const QString & from_id, const QString & to_id );

    /** \brief Check if an item exists in the database
      * \param id The id to check for in the database
      * \returns True if the item exists, false otherwise */
    static bool db_exists( const QString & id );

    /** \brief String representation for log files
      * \returns A String representation suitable for logging the item. */
    QString toString() const;

    /** \brief Apply a db event line
      * \param id Id of the item to add the event for
      * \param event The type of event
      * \param note Note. May be the empty string */
    static void addEventLine( const QString & id,
                             const QString & event,
                             const QString & note );

    /** \brief Apply a db event line
      * \param event The type of event
      * \param note Note. May be the empty string */
    void addEventLine( const QString & event,
                       const QString & note ) const;

private:
    QString m_id;
    QString m_type;
    QString m_size;
    QString m_mark;
    QString m_model;
    QString m_year;
    QString m_condition;
    qlonglong m_price;
    QString m_rentalgroup;
    QString m_state;
    QString m_note;

};

#endif // ITEM_H
