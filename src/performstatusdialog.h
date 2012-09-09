#ifndef PERFORMSTATUSDIALOG_H
#define PERFORMSTATUSDIALOG_H

#include <QDialog>

namespace Ui {
class PerformStatusDialog;
}

/** \brief A dialog window to help in performing status
  *
  * This dialog helps with performing status, that is, scanning all items
  * to figure out which items are present, and eventually, which are missing.
  * (A report is needed to figure out which are missing).
  *
  * The dialog also tracks if some items are scanned, which are not marked as in.
  * These can be printed, in order to allow returning them at a later point. */
class PerformStatusDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PerformStatusDialog(QWidget *parent = 0);
    ~PerformStatusDialog();
    
public slots:
    /** \brief signalled, whenever an item is scanned */
    void scan_item( const QString & item_id );

signals:
    /** \brief Emitted when an item was succesfully scanned */
    void item_scanned();

private slots:

    /** \brief Closes the dialog */
    void on_input_close_pushButton_clicked();

    /** \brief Allows to save item_ids for items that are present but not marked as in */
    void on_input_save_pushButton_clicked();

    /** \brief Allows to print item_ids for items that are present but not marked as in */
    void on_input_print_pushButton_clicked();

private:
    Ui::PerformStatusDialog *ui;
};

#endif // PERFORMSTATUSDIALOG_H
