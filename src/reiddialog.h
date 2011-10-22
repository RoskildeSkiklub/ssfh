#ifndef REIDDIALOG_H
#define REIDDIALOG_H

#include <QDialog>

namespace Ui {
    class ReIdDialog;
}

class ReIdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReIdDialog(QWidget *parent = 0);
    ~ReIdDialog();

public slots:
    void scan_item( const QString & item_id );

private slots:
    void on_input_lookup_pushButton_clicked();

private:
    Ui::ReIdDialog *ui;

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
