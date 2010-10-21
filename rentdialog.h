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

#include <QDialog>

// App
#include "contract.h"
class DKSundhedskort;


namespace Ui {
    class RentDialog;
}

class RentDialog : public QDialog {
    Q_OBJECT
private:
    Contract contract;

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
    void on_pushButton_clicked(bool checked);
};

#endif // RENTDIALOG_H
