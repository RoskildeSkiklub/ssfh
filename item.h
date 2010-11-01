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

    //! \todo Document properly
    // Construct an item instance from an id. Mark as booked in the database.
    static Item locate_and_book_in_db( const QString & id );

    /** \brief Insert this item into the database.
      * Will except on duplicate */
    void db_insert();

    /** \brief Return HTML representation of item */
    QString toHtml() const;

    /** \brief Default constructor */
    Item();

    /** \brief Return the id of the item
      * \return The id of the item */
    QString getId() const;

    /** \brief Return the rental group
      * \return The rental group of the item */
    QString getRentalGroup() const;

private:
    QString m_id;
    QString m_type;
    QString m_size;
    QString m_mark;
    QString m_model;
    QString m_year;
    QString m_condition;
    long m_price;
    QString m_rentalgroup;
    QString m_state;
    QString m_note;

};

#endif // ITEM_H
