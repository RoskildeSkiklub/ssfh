#ifndef RENTDIALOG_H
#define RENTDIALOG_H

/** \file
  * \brief Declare the main 'rent out' dialog.
  *
  * This dialog is the main dialog to perform rent out from, and therefore the
  * main GUI for users.
  *
  * It carries a contract, which is initially empty. When a hirer is assigned to the contract, items can be added.
  *
  * A statemachine is used to control the availability of buttons, etc.


  * A note on hirers:
  * Hirers can be set using a button event, or a scan event. The call chains are like this:
  *   on_input_hirer_pushButton_clicked => set_hirer( Hirer ) => try_set_hirer
  *   set_hirer( DKSundhedskort ) => set_hirer( Hirer ) => try_set_hirer

 */

// Qt
#include <QDialog>
#include <QStateMachine>

// App
#include "contract.h"
class DKSundhedskort;
namespace Globals {
    namespace BarcodeCommands {
        class Command;
    }
};


namespace Ui {
    class RentDialog;
}

class RentDialog : public QDialog {
    Q_OBJECT
private:
    /** \brief This holds the contract, which is mostly a struct like entity */
    Contract m_contract;

    /** \brief Our state machine. This is mostly used to control the availability of buttons, etc. */
    QStateMachine m_state_machine;

public:
    RentDialog(QWidget *parent = 0);
    ~RentDialog();

public slots:

    /** \brief Add an item to the contract
      * \param item_id The id of the item to add
      *
      * This method calls try_add_item and handles the returned value */
    void add_item( const QString & item_id );

    /** \brief Sets the hirer for the current contract
      * \param dsk Scan information with the hirer to set
      *
      * This method tries to match the scanned hirer to a hirer in the database.
      * If not found, the hirer is created. It then calls set_hirer. */
    void set_hirer( const DKSundhedskort & dsk );

    /** \brief Interprets the command, if applicable
      * \param command The command to interpret
      *
      * This method currently only reacts to "OperationDone", if applicable */
    void on_barcodeCommandScan( const Globals::BarcodeCommands::Command & command );

private slots:
    void on_input_editNote_pushButton_clicked();
    void on_input_finish_pushButton_clicked();
    void on_input_item_lineEdit_textChanged(QString );

    /** \brief Calls add_item */
    void on_input_addItem_pushButton_clicked();

    /** \brief Shows the hirer dialog, then calls set_hirer */
    void on_input_hirer_pushButton_clicked();

protected:
    void changeEvent(QEvent *e);

    /** \brief Only allow close, if the contract is parked or something */
    void closeEvent(QCloseEvent *);

    /** \brief Sets the hirer for the current contract
      * \param hirer The hirer to set
      *
      * If the dialog is in a state, where the hirer must not be set,
      * a dialog is shown, informing the user about this. */
    void set_hirer( const Hirer & hirer );

private:
    Ui::RentDialog *ui;
    /** \brief Update the ui to reflect the contract */
    // TODO : Should probably do this with a signal/slot thing....
    void update();

    bool try_add_item( const QString & item_id ); // Actually try to add the item.    

    /** \brief Check if the dialog is in a specific state
      * \param state The state to check for
      * \returns true if in that state, false otherwise
      *
      * \note This requires that all states are named */
    bool is_in_state( const QString & state );

    /** \brief Finish the rental
      *
      * This activates the contract, prints the receipt, and closes the window.
      * It is required that the dialog is in "has_item", state, otherwise
      * a warning is logged, and it is ignored */
    void finish();


signals:
    void hirer_set(); // emitted when a hirer has successfully been set on the contract
    void duration_changed(); // emitted when a contract has succesfully has its duration changed.
    void item_added(); // emitted when an item has succesfully been set on the contract
};

#endif // RENTDIALOG_H
