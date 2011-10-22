#ifndef REIDDIALOG_H
#define REIDDIALOG_H

#include <QDialog>

namespace Ui {
    class ReidDialog;
}

class ReidDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReidDialog(QWidget *parent = 0);
    ~ReidDialog();

public slots:
    void scan_item( const QString & item_id );

private slots:
    void on_input_lookup_pushButton_clicked();

private:
    Ui::ReidDialog *ui;

    /** \brief Method to lookup the item when the from id has been set
      *
      * This also stops further input to the item lineedit.
      * It throws, and is exception safe, if item does not exist
      * */
    void fromIdSet();

    /** \brief Method to actually perform the reid
      *
      * This calles the Item::db_reid method and prepares for a new go */
    void toIdSet();


};

#endif // REIDDIALOG_H
