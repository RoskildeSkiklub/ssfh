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

    //! \todo Document
    /** \brief Locate and load an item from the database
      * \param id The id of the item to load */
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

    /** \brief Default constructor */
    Item();

    /** \brief Return the id of the item
      * \return The id of the item */
    QString getId() const;

    /** \brief Return the rental group
      * \return The rental group of the item */
    QString getRentalGroup() const;

    /** \brief Get the state of the item
      * \return The state of the item */
    QString getState() const;


    /** \brief Update the state of this item in the database to "out"
      *
      * This updates the state in the database, and also updates the in-memory state
      * The state may not already be out */
    void setToOutInDatabase();

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

    /** \brief String representation for log files */
    QString toString() const;
};

#endif // ITEM_H
