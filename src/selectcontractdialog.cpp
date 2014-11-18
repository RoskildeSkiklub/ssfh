//
// Snow Stuff For Hire - A POS-like system designed for very quick rental
// operations at small ski resorts
// Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
//
// This file is part of Snow Stuff For Hire.
//
// Snow Stuff For Hire is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Snow Stuff For Hire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
//

#include "selectcontractdialog.h"
#include "ui_selectcontractdialog.h"

// Qt
#include <QSqlQuery>
#include <QVariant>

// App
#include "utility.h"
#include "log.h"
#include "contract.h"
using namespace Log;

SelectContractDialog::SelectContractDialog(const QString & state, QWidget *parent) :
    QDialog(parent),
    m_state( state ),
    m_contract_id( -1 ),
    ui(new Ui::SelectContractDialog)
{
    ui->setupUi(this);

    // TODO: Check that state is legal...

    // Setup the query model
    QSqlQuery query;
    query_check_prepare( query, "select contracts.id, hirers.firstname, hirers.lastname from contracts, hirers "
                         "where contracts.state =:state "
                         "and hirers.id = contracts.hirer "
                         "order by firstname, lastname " );
    query.bindValue( ":state", state);
    query_check_exec( query );
    model.setQuery( query );
    model.setHeaderData( 0, Qt::Horizontal, tr( "Id" ) );
    model.setHeaderData( 1, Qt::Horizontal, tr( "First name") );
    model.setHeaderData( 2, Qt::Horizontal, tr( "Last name") );
    ui->input_contract_tableView->setModel( & model );
    ui->input_contract_tableView->resizeColumnsToContents();

}

SelectContractDialog::~SelectContractDialog()
{
    delete ui;
}

void SelectContractDialog::on_input_contract_tableView_activated(QModelIndex index) {
    Logger log("void SelectContractDialog::on_input_contract_tableView_activated(QModelIndex index)");
    // Get id from coloum 0
    QModelIndex myIndex = model.index( index.row(), 0 );
    qlonglong contract_id = model.data( myIndex ).toLongLong();
    log.stream() << "Selected contract with id '"
                << contract_id << "'";
    // Load it, set the text
    ui->output_contract_textBrowser
            ->setHtml( Contract::db_load( contract_id ).toReturnHtml() );
    m_contract_id = contract_id;
}
