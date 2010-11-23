#ifndef CONTRACT_H
#define CONTRACT_H

// Qt
#include <QList>
#include <QDateTime>

// In memory representation of a contract.

// App
#include "hirer.h"
#include "item.h"
#include "contractitem.h"

namespace Pos {
    class Printer;
};

/** \brief In memory representation *and* business logic class for contracts
  *
  * This class represents a contract from it is created, while it is updated,
  * and after beeing committed to the database.
  *
  * Initially, the contract is created by e.g. the rent dialog. In this state, it
  * is not valid, has no hirer, no duration (or the default duration), and no items.
  *
  * After beeing created, a hirer can be set. The contract is still not valid at this
  * point, but now a duration can be set. After a duration is set, items can be added.
  *
  * Items can be added and removed, if state is booking.
  * Items can change rentalgroup, if state is booking.
  *
  * Discount can be set and read.
  * The total amount that needs payment can be retrieved, as can the total price.
  * The amount payed so far can be retrived. Payments can be performed.
  *
  * In the database, and reflected in the m_state field, the contract can be in one of
  * four states:
  * booking: The contract is beeing created, more items can be added
  * parked: Almost as booking, but used when the contract is not currently in the GUI
  * active: The contract has been finalized with the hirer, and is currently in progress.
  * closed: All items on the contract has been returned.
  *
  * The contract can have it state changed, by using the appropiate state change functions */
class Contract {
    Q_DECLARE_TR_FUNCTIONS(Contract)
public:
    /** \brief Default constructor.
      *
      * Sets the id to -1, the state to booking and does not create anything in the database */
    Contract();

    /** \brief Sets the hirer
      * \param hirer The hirer to set
      * \returns true if set, false if hirer is already set (and valid) on this contract.
      *
      * Throws, if the hirer passed in the argument is invalid, or if the state of
      * the contract is not 'booking' */
    bool setHirer( const Hirer & hirer );

    /** \brief Return a reference to the current hirer
      * \return A reference to the current hirer */
    Hirer & getHirer();

    /** \brief Set the contract duration
      * \param start The start of the contract period
      * \param end The end of the contract period
      *
      * This is only used to figure out the pricerules. Will call recalculatePrices.
      * Throws if the state is not booking */
    void setDuration( const QDateTime & start, const QDateTime & end );

    /** \brief Recalculate all prices
      *
      * Recalculate the prices of each item, if the contract is not closed */
    void recalculatePrices();

    /** \brief Add an item to the contract.
      * \param item_id Id of the item to add
      *
      * This method adds an item. A number of controls are performed
      * 1) if the item is already on the contract, it can not be added
      * 2) if the item does not exist, some error happens
      * 3) if the item is not available (not in), an error happens.
      *
      * Errors are returned as Exceptions with a specific statuscode.
      * If the state of the contract is not 'booking', an error is thrown.
      *
      * Note, the first addItem on a contract also creates the contract in the database.
      * This is because contract items needs to be created that points at this contract.
      * TODO: This may, or may not, be needed. */
    void addItem( const QString & item_id );

    /** \brief Return an item
      * \param item_id The id of the item to be returned
      *
      * This methods return an item.
      * It is an error, if the state of the contract is not active
      * It is checked that the item is actually part of this contract.
      * If the item is not actually out, it is silently ignored, but a warning is logged */
    void returnItem( const QString & item_id );

    /** \brief Remove an item from the contract
      * \param item_id Id of the item to remove
      *
      * If the state of the contract is 'booking', the item is removed
      * and marked as 'in'. All prices are recalculated */
    void removeItem( const QString & item_id );

    /** \brief Change the rental group of an item
      * \param item_id Id of the item to change
      * \param rentalGroup The new rental group to change to
      *
      * If the state of the contract is 'booking', then the rentalgroup
      * of the item is changed to the new rentalgroup. All prices are recalculated */
    void changeItemRentalGroup( const QString & item_id, const QString & rentalGroup );

    /** \brief Returns true if there are still items in the out state, false otherwise
      * \return True if there are items in the outstanding state, false otherwise
      *
      * TODO: Handle items that are lost, stuff like that */
    bool hasReturnableItems() const;

    /** \brief Get the id of the contract
      * \return The id of the contract */
    qlonglong getId() const;

    /** \brief Get the state of the contract
      * \return The state of the contract */
    QString getState() const;

    /** \brief Set the discount
      * \param discount
      *
      * If the state of the contract is 'booking', then the discount is changed.
      * All prices are recalculated. */
    void setDiscount( qlonglong discount );

    /** \brief Get the current discount
      * \returns The current discount */
    qlonglong getDiscount() const;

    /** \brief Get the price of all items
      * \returns The price of all items added to the contract
      *
      * This is the sum of the price of all items */
    qlonglong getItemsPrice() const;

    /** \brief Get the total price of the contract
      * \returns The total price of the contract
      *
      * This is getItemsPrice() - getDiscount() */
    qlonglong getTotalPrice() const;

    /** \brief Get the current payable sum.
      * \returns The current payable sum
      *
      * This is getItemsPrice - getDiscount() - getPayed(), that is,
      * the current amount 'missing' for the user to fullfill the contract price */
    qlonglong getPayableAmount() const;

    /** \brief Get the amount already payed by the hirer
      * \returns the amount allready payed by the hirer */
    qlonglong getPayed() const;

    /** \brief Get the note of the contract
      * \return Copy of the note of the contract */
    QString getNote() const;

    /** \brief Set the note on the contract
      * \param note The new note on the contract */
    void setNote( const QString & note );

    /** \brief Add a payment to the contract
      * \param amount The amount to add to payed
      *
      * If the contract is not in state 'booking' an error is thrown */
    void addPayment( qlonglong amount );

    /** \brief Park the contract
      *
      * This changes the state to park. This is mostly useful in order to postpone
      * the handling of the contract in the GUI.
      * You can only park a contract in the booking state. */
    void park();

    /** \brief Activate the contract
      *
      * This state change should be performed, when the user have payed the
      * contract. The state must be booking, and if there are outstanding payments,
      * an exception is thrown.
      *
      * The contract, and all contract items are written to the database.
      * The status of all items are also written, updated to be "out"*/
    void activate();

    /** \brief Close the contract
      *
      * This should be called, when all items in the contract has been returned.
      * The contract is then marked closed.
      * If any items are still outstanding, an exception is thrown */
    void close();

    /** \brief Cancel the contract
      *
      * This method "returns" any items and in general "resets" everything.
      * Only works on contracts in 'booking' state... */
    void cancel();

    /** \brief Load a contract, inclusive contractItems from the database
      * \param id The id of the contract
      *
      * This method loads the contract and all contractitems from the database. */
    static Contract db_load( qlonglong id );

    /** \brief Return a HTML representation of the contract for the rental window
      * \return HTML representation of the contract.
      *
      * The HTML includes hints on what to do, which I should probably drop again */
    QString toRentalHtml() const;

    /** \brief Return a HTML representation of the contract for the rental window
      * \return HTML representation of the contract. */
    QString toReturnHtml() const;

    /** \brief Print rental agreement to a PosPrinter
      * \param printer The pos printer instance to use
      *
      * Call this method, when renting out the items, after done.
      * The contract must be active, when this method is called */
    void printRental( Pos::Printer & posPrinter );

    /** \brief Print rental receipt to a PosPrinter
      * \param printer The pos printer instance to use
      *
      * Call this method in order to produce a receipt for the customer after rental.
      * The contract must be active, when this method is called */
    void printReceipt( Pos::Printer & posPrinter );

    /** \brief Print return receipt to a PosPrinter
      * \param printer The pos printer instance to use
      *
      * Call this method, when all items have been returned, and the contract have been closed.
      * The contract must be closed, when this method is called */
    void printReturn( Pos::Printer & posPrinter );

private:
    /** \brief Database field: The id of the contract. Initially -1 */
    qlonglong m_id;

    /** \brief Database field: The creation time of the contract */
    QDateTime m_creationTime;

    /** \brief Database field: The start time of the rental period */
    QDateTime m_startTime;

    /** \brief Database field: The end time of the renta period */
    QDateTime m_endTime;

    /** \brief Database field: Any discount on the contract */
    qlonglong m_discount;

    /** \brief Database field: The total price of the contract: Sum of items - discount */
    qlonglong m_price;

    /** \brief Database field: Amount payed by hirer so far */
    qlonglong m_payed;

    /** \brief Database field: State of contract.
      *
      * One of 'booking', 'parked', 'active', 'closed' */
    QString m_state;

    /** \brief Database field: Any notes on the contract */
    QString m_note;

    /** \brief The hirer for the contract, set by setHirer */
    Hirer m_hirer;

    /** \brief The items added to the contract. */
    QList<ContractItem> m_contractItems;

    /** \brief Checks the contract is in booking state, throws otherwise
      * \param method The name of the method calling this method, used for error reporting */
    void checkInBookingState( const QString & method );

    /** \brief Checks the contract is in active state, throws otherwise
      * \param method The name of the method calling this method, used for error reporting */
    void checkInActiveState( const QString & method );

    /** \brief Checks the contract is in closed state, throws otherwise
      * \param method The name of the method calling this method, used for error reporting */
    void checkInClosedState( const QString & method );

    /** \brief Calculates the price of a single item on this contract
      * \param item The item to calculate for
      * \return The price of the item in this contract */
    qlonglong calculateItemPrice( const Item & item );

    /** \brief Insert a copy of this object into the database.
      *
      * This method checks that the contract is in booking state, and that id is -1,
      * and then inserts a copy of the object into the database. It then reads back
      * the id from the database, and updates the id. */
    void db_insert();

    /** \brief Updates the contract in the database
      *
      * Updates all columns in the database, except id and creation time,
      * with current values in memory
      * Throws in case of errors */
    void db_update();

    // Returns a common representation
    QString toCommonHtml( bool isRental ) const;

    /** \brief String representation for log files
      *
      * Only the base part, not hirer and contract item */
    QString toString() const;

    typedef enum {
        RentalAgreement = 0,
        RentalReceipt = 1,
        ReturnReceipt = 2
    } ReceiptType;

    /** \brief Print a rental agreement or an receipt */
    void printGeneralReceipt( Pos::Printer & posPrinter, ReceiptType receiptType );
};

#endif // CONTRACT_H
