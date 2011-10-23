#ifndef REIDDIALOG_H
#define REIDDIALOG_H

#include <QDialog>
class QAbstractButton;

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

    void on_input_fromId_lineEdit_textChanged(QString );

    void on_input_toId_lineEdit_textChanged(QString );

    void on_buttonBox_accepted();

    void on_buttonBox_clicked(QAbstractButton* button);

    void on_input_loadFile_pushButton_clicked();

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
    bool doReId();

    /** \brief Clear the dialog, prepare for a new reid operation */
    void clear();

    /** \brief Read id changes from a file, and process them */
    void readChangesFromFile();



};

#endif // REIDDIALOG_H
