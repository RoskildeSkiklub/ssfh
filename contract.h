#ifndef CONTRACT_H
#define CONTRACT_H

// Qt
#include <QList>
#include <QDateTime>

// In memory representation of a contract.

// App
#include "hirer.h"
#include "item.h"

class Contract {
    Hirer m_hirer;
public:
    QList<Item> m_items;

    qlonglong m_id;
    bool m_active;
    QDateTime startTime;
    QDateTime endTime;
    long getPrice();
    bool m_payed;
    QString m_note;

    /** \brief Add an item to the contract.
      * \param item_id Id of the item to add
      *
      * This method adds an item. A number of controls are performed
      * 1) if the item is already on the contract, it can not be added
      * 2) if the item does not exist, some error happens
      * 3) if the item is not available (not in), an error happens.
      *
      * Errors are returned as Exceptions with a specific statuscode. */
    void add_item( const QString & item_id );

    Hirer & getHirer();
    /** \brief Sets the hirer
      * \param hirer The hirer to set
      * \returns true if set, false if hirer is already valid.
      *
      * Throws, if the hirer passed in the argument is invalid. */
    bool setHirer( const Hirer & hirer );

    // Return a html representation of the items
    QString items_to_html() const;

    /** \brief Return a HTML representation of the contract
      * \return HTML representation of the contract.
      *
      * The HTML includes hints on what to do, which I should probably drop again */
    QString toHtml() const;


    Contract();
};

#endif // CONTRACT_H
