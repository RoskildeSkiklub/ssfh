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
      *
      * This updates the state in the database, and also updates the in-memory state
      * The state may not already be out */
    void db_setToOut();

    /** \brief Update the state of this item in the database to "in"
      *
      * This updates the state in the database, and also updates the in-memory state */
    void db_setToIn();

    /** \brief Update only the state of the item.
      * \param state The new state
      *
      * This method updates only the state in the database, and in
      * the in memory representation.
      * It will throw, if the state is not known or illegal for this method.
      * 'out' and 'booked' are illegal for this method. */
    void db_forceState( const QString & state );
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

    /** \brief Apply a db event line
      * \param event The type of event
      * \param note Optional note */
    void addEventLine( const QString & event,
                       const QString & note ) const;



};

#endif // ITEM_H
