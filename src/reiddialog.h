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

signals:
    void item_scanned();

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
