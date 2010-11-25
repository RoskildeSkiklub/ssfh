#ifndef ADDNEWITEMDIALOG_H
#define ADDNEWITEMDIALOG_H

// Qt
#include <QDialog>
#include <QStack>

// App
#include "item.h"

namespace Ui {
    class AddNewItemDialog;
}

class AddNewItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewItemDialog(QWidget *parent = 0);
    ~AddNewItemDialog();

public slots:
    void add_item( const QString & item_id ); // Add an item

private:
    Ui::AddNewItemDialog *ui;

    /** \brief Stack of items added, used for undo. No redo support. */
    QStack<Item> items_added;

    /** \brief Setup form, based on values in type. Cascades */
    void doSetupBasedOnType();

    /** \brief Setup form, based on values in mark. Cascades */
    void doSetupBasedOnMark();

    /** \brief Add an item, based on settings in dialog, and id */
    void doAddItem( QString id );

    /** \brief Update the log window from the stack contents */
    void updateLogWindow();

    /** \brief Undo the last item inserted */
    void doUndo();

private slots:
    void on_input_undo_pushButton_clicked();
    void on_input_done_pushButton_clicked();
    void on_input_id_lineEdit_textChanged(QString );
    void on_input_add_pushButton_clicked();
    void on_input_mark_comboBox_currentIndexChanged(QString );
    void on_input_type_comboBox_currentIndexChanged(QString );
};

#endif // ADDNEWITEMDIALOG_H
