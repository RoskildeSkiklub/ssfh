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
