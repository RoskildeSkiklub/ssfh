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

#ifndef CONTRACTITEMS_H
#define CONTRACTITEMS_H

// Qt
#include <QString>

// App
#include "item.h"

class ContractItem {
     Q_DECLARE_TR_FUNCTIONS(Item);

public:

    /** \brief Default constructor.
      *
      * Constructs empty, invalid, in-memory only ContractItem */
    ContractItem();

    /** \brief Create a ContractItem from certain set of values
      * \param contract_id The id of the contract
      * \param item The item for this contract
      * \param rentalgroup The rental group of the item on this contract
      * \param price The price of the item on this contract
      * \param note A note for the item
      *
      * This constructor construct with the given values, and state = 'booked'.
      * It is *not* written to the database. Use insert for that */
    ContractItem( qlonglong contract_id, const Item & item,
                  const QString & rentalgroup, qlonglong price,
                  const QString & note );

    /** \brief Return some HTML suitable to show in the rental dialog
      * \returns HTML suitable for show in the rental dialog */
    QString toRentalHtml() const;

    /** \brief Return some HTML suitable to show in the return dialog
      * \returns HTML suitable for show in the return dialog */
    QString toReturnHtml() const;

    /** \brief Get the id of the contract item
      * \return The id of the contract item */
    qlonglong getId() const { return m_id; }

    /** \brief Set the if of the contract item
      *
      * \todo Fix this to unpark, something */
    void setId( qlonglong id ) { m_id = id; }

    /** \brief Get a reference to the item
      * \return A reference to the item */
    Item & getItem() { return m_item; }

    /** \brief Get a reference to the item
      * \return A reference to the item */
    const Item & getItem() const { return m_item; }

    /** \brief Get the price of this item on this contract
      * \return The price of this item on this crontact */
    qlonglong getPrice() const { return m_price; }

    /** \brief Get the rental group of the item in this contract
      * \return The rental group of the item in this contract */
    QString getRentalgroup() const { return m_rentalgroup; }

    /** \brief Get the state
      * \return The state of the contractitem
      *
      * The states can be one of 'booked', 'out' or 'returned' */
    QString getState() const { return m_state; }

    /** \brief Get the contract id of this contract item
      * \return The contract id of this item */
    qlonglong getContractId() const { return m_contract_id; }

    /** \brief Update the state
      * \param state The new state
      *
      * Changes the internal representation of state */
    void setState( const QString & state );

    /** \brief Insert the contract item, as-is, into the database.
      *
      * Reads back the new id */
    void db_insert();

    /** \brief Load a ContractItem from the database.
      * \param id The id of the ContractItem
      * \returns A new ContractItem, representing the data in the database
      *
      * This method loads the ContractItem from the database, and also the associated item! */
    static ContractItem db_load( qlonglong id );

    /** \brief Updates the contract item in the database
      * \param new_state The new state of the contract item
      *
      * This method writes the new state to the database, and updates the in-memory representation. */
    void db_update_state( const QString & new_state );

private:
    /** \brief Database field: Id of the contract item in the database */
    qlonglong m_id;
    /** \brief Database field: Id of the contract in the database */
    qlonglong m_contract_id;
    /** \brief Copy of item assigned to this contractitem.
      * Only item id is stored in database */
    Item m_item;
    /** \brief State of contractitem
      *
      * booked: Still booking/reserved for this contract
      * out: Out - that is, in the care of the hirer
      * returned: Returned. */
    QString m_state;
    /** \brief Database field: The rental group registered for this item on this contract */
    QString m_rentalgroup;
    /** \brief Database field: The price registered for this item on this contract */
    qlonglong m_price;
    /** \brief Database field: A note */
    QString m_note;

    /** \brief String representation for log files
      *
      * Only the base part, not items */
    QString toString() const;

};

#endif // CONTRACTITEMS_H
