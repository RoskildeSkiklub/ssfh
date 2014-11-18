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

#ifndef SELECTCONTRACTDIALOG_H
#define SELECTCONTRACTDIALOG_H

// Qt
#include <QDialog>
#include <QString>
#include <QSqlQueryModel>


namespace Ui {
    class SelectContractDialog;
}

class SelectContractDialog : public QDialog
{
    Q_OBJECT
    QString m_state; // The state of the contracts to show
    qlonglong m_contract_id;

public:
    explicit SelectContractDialog( const QString & state, QWidget *parent = 0);
    ~SelectContractDialog();

    /** \brief Get the id of the selected contract.
      * \return The id of the selectec contract, or -1 if no contract selected */
    qlonglong getContractId() const {  return m_contract_id; }

private:
    /** \brief The model that holds the data about the contracts */
    QSqlQueryModel model;

    Ui::SelectContractDialog *ui;

private slots:
    void on_input_contract_tableView_activated(QModelIndex index);
};

#endif // SELECTCONTRACTDIALOG_H
