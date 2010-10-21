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
public:
    Hirer m_hirer;
    QList<Item> m_items;

    qlonglong m_id;
    bool m_active;
    QDateTime startTime;
    QDateTime endTime;
    long getPrice();
    bool m_payed;
    QString m_note;

    // Methods...
    void add_item( const QString & item_id );

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
