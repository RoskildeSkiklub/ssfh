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

#ifndef VIEWHIREDITEMSDIALOG_H
#define VIEWHIREDITEMSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>


namespace Ui {
    class ViewHiredItemsDialog;
}

class ViewHiredItemsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewHiredItemsDialog(QWidget *parent = 0);
    ~ViewHiredItemsDialog();

private:
    Ui::ViewHiredItemsDialog *ui;

    /** \brief The model that holds the items data */
    QSqlQueryModel model;



private slots:
    void on_pushButton_clicked();
    void on_item_tableView_activated(QModelIndex index);
};

#endif // VIEWHIREDITEMSDIALOG_H
