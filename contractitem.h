#ifndef CONTRACTITEMS_H
#define CONTRACTITEMS_H

// Qt
#include <QString>

// App
#include "item.h"

class ContractItem
{
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

    /** \brief Get a reference to the item
      * \return A reference to the item */
    Item & getItem() { return m_item; }

    /** \brief Get the price of this item on this contract
      * \return The price of this item on this crontact */
    qlonglong getPrice() { return m_price; }

    /** \brief Get the state
      * \return The state of the contractitem
      *
      * The states can be one of 'booked', 'out' or 'returned' */
    QString getState() const { return m_state; }

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
