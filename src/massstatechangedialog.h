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

#ifndef MASSSTATECHANGEDIALOG_H
#define MASSSTATECHANGEDIALOG_H

/** \file
  * \brief The mass state change dialog is commonly used to return items that have not been properly returned */

// Qt
#include <QDialog>
class QString;

namespace Ui {
    class MassStateChangeDialog;
}

class MassStateChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MassStateChangeDialog(QWidget *parent = 0);
    ~MassStateChangeDialog();

private:
    Ui::MassStateChangeDialog *ui;

private slots:

    void on_input_close_pushButton_clicked();
    void on_input_doIt_pushButton_clicked();

public slots:

    /** \brief Update the state of an item with the current selected state
      * \param item_id The id of the item to update */
    void update_item( const QString & item_id );

};

#endif // MASSSTATECHANGEDIALOG_H
