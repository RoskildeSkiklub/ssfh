#ifndef RETURNDIALOG_H
#define RETURNDIALOG_H

/** \file
  * \brief Declare the main 'return' dialog
  *
  * This dialog is for the main dialog to use, when items are returned.
  *
  * It carries a contract which is initially empty.
  * When a contract is selected, or an item returned, the contract is displayed in
  * the text browser window
  *
  * A statemachine is used to control the availability of buttons, etc.
  */




// Qt
#include <QDialog>
#include <QStateMachine>

// App
#include "contract.h"

namespace Ui {
    class ReturnDialog;
}

class ReturnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReturnDialog(QWidget *parent = 0);
    ~ReturnDialog();

private:
    Ui::ReturnDialog *ui;

    /** \brief This holds the contract, which is mostly a struct like entity */
    Contract m_contract;

    /** \brief Our state machine. This is mostly used to control the availability of buttons, etc. */
    QStateMachine m_state_machine;

    /** \brief Update the display */
    void update();

    /** \brief Check if the dialog is in a specific state
      * \param state The state to check for
      * \returns true if in that state, false otherwise
      *
      * \note This requires that all states are named */
    bool is_in_state( const QString & state );

public slots:
    /** \brief Return an item on the contract
      * \param item_id The id of the item to return
      *
      * This method calls try_return_item and handles the returned value */
    void return_item( const QString & item_id );

signals:
    void contract_found(); // Emitted when a contract is set on the dialog
    void item_returned(); // Emitted when an item is returned
    void contract_closed(); // Emitted when a contract has been fully closed

private slots:
    void on_input_returnAll_pushButton_clicked();
    void on_input_done_pushButton_clicked();
    void on_input_returnItem_pushButton_clicked();

protected:
    /** \brief Only allow close, if users says OK */
    void closeEvent(QCloseEvent *);

};

#endif // RETURNDIALOG_H
