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

#ifndef HIRERDIALOG_H
#define HIRERDIALOG_H

/** \file
  * \brief This defines a dialog that can be used to locate, create, edit and/or select a hirer.
  *
  * This dialog probably does too much currently. It can be used to locate a hirer, to create/add a hirer,
  * to update the values for a given hirer, and finally to select a hirer, that the client can use.
  *
  * It also reacts to a CreditCard signal, and updates the values with the information passed in that, storing
  * the card number as an ssn ... */

#include <QDialog>
#include <QSqlQueryModel>
#include <QSplitter>
#include <QWidget>
#include <QGridLayout>
#include <QStateMachine>

// App
#include "contract.h"
#include "hirer.h"
#include "interceptor.h"

namespace Ui {
    class HirerDialog;
}

class HirerDialog : public QDialog {
    Q_OBJECT
private:
    /** \brief The model that links the running query to the display on the form */
    QSqlQueryModel *model;

    /** \brief The hirer is stored here, if the user selects one. */
    Hirer hirer;

public:
    HirerDialog(QWidget *parent = 0);
    ~HirerDialog();

    /** \brief Get the current hirer
      * \return The current hirer
      *
      * If the dialog is executed and returns accepted, the client can get the hirer by using this method. */
    Hirer getHirer();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::HirerDialog *ui;

    /** \brief The horizontal splitter */
    QSplitter * splitter;

    /** \brief The widget to hold the top layout */
    QWidget * top_widget;

    /** \brief I would think I could get along without this - but I can't figure out how. */
    QGridLayout * top_grid_layout;

    /** \brief Our state machine. This is mostly used to control the avialability of buttons, etc. */
    QStateMachine state_machine;

    /** \brief Updates the query with the values in the edit lines, and rerun it. */
    void updateQueryModel();

    /** \brief Loads a hirer based on the id, into the edit line values
      * \param id The id of the hirer, in the database. */
    void loadHirer( qlonglong id );

private slots:
    void on_pushButton_use_clicked();
    void on_checkBox_autoLookup_stateChanged(int );
    void on_pushButton_update_clicked();
    void on_pushButton_add_clicked();
    void on_pushButton_clear_clicked();
    void on_tableView_hirers_doubleClicked(QModelIndex index);
    void on_pushButton_lookup_clicked();

    // My stuff.
    void on_input_field_edited( QString value );
    void on_note_field_changed();

public slots:
    void on_CreditCard_magSwipe( const CreditCard & creditCard );
    void on_barcodeSSNScan( const QString & ssn );

signals:
    void input_field_edited();
    void select_hirer(); // Emitted when a hirer is selected
};

#endif // HIRERDIALOG_H
