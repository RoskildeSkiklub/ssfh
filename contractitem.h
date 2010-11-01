#ifndef CONTRACTITEMS_H
#define CONTRACTITEMS_H

#include <QString>

class ContractItem
{
public:
    qlonglong m_id;
    qlonglong m_contract_id;
    QString m_item_id;
    QString m_state;
    QString m_rentalgroup;
    qlonglong m_price;
    QString m_note;

    /** \brief Default constructor.
      *
      * Constructs empty, invalid, in-memory only ContractItem */
    ContractItem();

    /** \brief Create a ContractItem from certain set of values
      * \param contract_id The id of the contract
      * \param item_id The The id of the item
      * \param rentalgroup The rental group of the item on this contract
      * \param price The price of the item on this contract
      * \param note A note for the item
      *
      * This constructor construct with the given values, and state = 'booked'.
      * It is *not* written to the database. Use insert for that */
    ContractItem( qlonglong contract_id, const QString & item_id,
                  const QString & rentalgroup, qlonglong price,
                  const QString & note );

};

#endif // CONTRACTITEMS_H
