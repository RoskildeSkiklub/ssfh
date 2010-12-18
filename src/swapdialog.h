#ifndef SWAPDIALOG_H
#define SWAPDIALOG_H

// Qt
#include <QDialog>
#include <QString>
class QAbstractButton;

// app
#include "contract.h"

namespace Ui {
    class SwapDialog;
}

class SwapDialog : public QDialog
{
    Q_OBJECT
private:
    /** \brief Holds the contract, when the first item has been scanned */
    Contract m_contract;

    /** \brief Method to lookup the contract when the first item has been set
      *
      * This also stops further input to the item lineedit.
      * It throws, and is exception safe, if item is not actually part of a contract, or similar.
      * */
    void returnItemSet();

    /** \brief Method to actually perform the swap
      *
      * This returns the "returnItem", and rents the "rentItem", the prints a receipt, and closes the dialog */
    void rentItemSet();

    /** \brief Sets up the contract, based on the value of the ui return item */
    bool locateContract( QString item_id );

public:
    explicit SwapDialog(QWidget *parent = 0);
    ~SwapDialog();

signals:
    void item_returned();
    void item_added();

public slots:
    void scan_item( const QString & item_id );

private:
    Ui::SwapDialog *ui;

private slots:
    void on_buttonBox_clicked(QAbstractButton* button);
    void on_input_rentItem_lineEdit_textChanged(QString );
    void on_buttonBox_accepted();
    void on_input_returnItem_lineEdit_textChanged(QString );
    void on_input_lookup_pushButton_clicked();



};

#endif // SWAPDIALOG_H
