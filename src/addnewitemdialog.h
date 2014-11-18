/* * Snow Stuff For Hire - A POS-like system designed for very quick rental
 * operations at small ski resorts
 * Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
 *
 * This file is part of Snow Stuff For Hire.
 *
 * Snow Stuff For Hire is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Snow Stuff For Hire is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
 */

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
