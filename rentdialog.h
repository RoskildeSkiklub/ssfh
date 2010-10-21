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
  * A statemachine is used to control the availability of buttons, etc. */

// Qt
#include <QDialog>
#include <QStateMachine>

// App
#include "contract.h"
class DKSundhedskort;


namespace Ui {
    class RentDialog;
}

class RentDialog : public QDialog {
    Q_OBJECT
private:
    /** \brief This holds the contract, which is mostly a struct like entity */
    Contract contract;

    /** \brief Our state machine. This is mostly used to control the avialability of buttons, etc. */
    QStateMachine state_machine;

public:
    RentDialog(QWidget *parent = 0);
    ~RentDialog();

public slots:
    void on_addButton_clicked(); // Click a button
    void add_item( const QString & item_id ); // Add an item
    void set_hirer( const DKSundhedskort & dsk ); // Set the hirer

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RentDialog *ui;
    bool try_add_item( const QString & item_id ); // Actually try to add the item.
    bool try_set_hirer( const DKSundhedskort & dsk ); // Actually try to add the hirer.

private slots:
    void on_input_hirer_pushButton_clicked();

signals:
    void hirer_set(); // emitted when a hirer has successfully been set on the contract
    void duration_changed(); // emitted when a contracts has succesfully has its duration changed.
    void item_added(); // emitted when an item has succesfully been set on the contract
};

#endif // RENTDIALOG_H
