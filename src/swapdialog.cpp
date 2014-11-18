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

#include "swapdialog.h"
#include "ui_swapdialog.h"

// Qt
#include <QMessageBox>
#include <QSqlQuery>
#include <QVariant>

// App
#include "log.h"
#include "exception.h"
#include "item.h"
#include "db_consts.h"
#include "utility.h"
#include "globals.h"

using namespace Log;

SwapDialog::SwapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SwapDialog)
{
    ui->setupUi(this);
    // Disable accept/ok button
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    // Get the signals from the scan input events.
    connect( Globals::interceptor, SIGNAL(barcodeItemScan(QString)),
             this, SLOT(scan_item(QString) ) );
    // Provide audio feedback when returning/adding items
    connect(this, SIGNAL(item_returned()),
            Globals::getFeedbackObject(), SLOT(itemReturned()));
    connect(this, SIGNAL(item_added()),
            Globals::getFeedbackObject(), SLOT(itemAdded()));
}

SwapDialog::~SwapDialog()
{
    delete ui;
}

void SwapDialog::scan_item(const QString &item_id) {
    Logger log("SwapDialog::scan_item(const QString &item_id)");
    // Use readonly status of returnItem to determine where we are...

    if ( ui->input_returnItem_lineEdit->isReadOnly() ) {
        log.stream() << "returnItem is readonly, inputting stuff into rentItem";
        // Insert into rentItem and do the swap.
        ui->input_rentItem_lineEdit->setText( item_id );
        rentItemSet();
        emit item_added();
    } else {
        log.stream() << "returnItem is writeable, inputting stuff into returnItem";
        // Into returnItem, and try to return
        ui->input_returnItem_lineEdit->setText( item_id );
        returnItemSet();
        emit item_returned();
    }
}

bool SwapDialog::locateContract( QString item_id ) {
    Logger log("void SwapDialog::locateContract()");

    // TODO:::: This code is lifted from the return dialog. It should be refactored into a common something, e.g. Contract::db_load( item_id ) or something.

    // Construct an item based on the id
    Item item( Item::db_load( item_id ) );
    // Check item is actually out
    if ( item.getState() != DB::Item::State::out ) {
        log.stream( warn ) << "When returning the item with id '" << item_id
                << "' it was marked as '" << item.getState() << "'";
        QMessageBox::critical( this, tr( "Item not out"),
                               tr("Item with id '%0' is in state '%1'. "
                                  "Use maintenance to change state of item.")
                               .arg( item_id ).arg( item.getState() ) );
        return false;
    }
    log.stream() << "Item loaded, and is out. Checking for contract.";
    QSqlQuery query;
    query_check_prepare( query,
                         "select id, contract_id from contractitems "
                         "where item_id=:item_id "
                         "and state=:state " );
    query.bindValue( ":item_id", item.getId() );
    query.bindValue( ":state", DB::ContractItem::State::out );
    query_check_exec( query );
    try {
        query_check_first( query );
    } catch ( const Exception & e ) {
        if ( e.getStatusCode() == Errors::DBResultError ) {
            log.stream( warn ) << "When returning the item with id '" << item_id
                    << "' it was marked as '" << DB::Item::State::out
                    << "', but no contractitems in state '"
                    << DB::ContractItem::State::out
                    << "' associated with the item was found. ";
            QMessageBox::critical( this, tr( "No contract found"),
                                   tr("Unable to locate a current contract for item with id '%0'. "
                                      "Use maintenance to change state of item.").arg( item_id ) );
            return false;

        }
    }

    // TODO: If fails, item is marked out, but no contract items
    qlonglong contract_id = query.value( 1 ).toLongLong();
    log.stream() << "Found contractitem on item, with id: ''"
            << query.value( 0 ).toLongLong()
            << "' and contract_id: '"
            << contract_id << "'";
    Contract contract( Contract::db_load( contract_id ) );
    if ( DB::Contract::State::active != contract.getState() ) {
        throw Exception( Errors::DBConsistencyError )
                << ( log.stream( error )
                     << "The item with id '" << item_id << "' had state '"
                     << item.getState() << "' but contract with id '"
                     << contract_id << "' had state '" << contract.getState() << "'. Aborting item return." );
    }

    // Its ok - just set it up.
    m_contract = contract;
    return true;
}


void SwapDialog::returnItemSet() {
    Logger log("SwapDialog::returnItemSet()");
    if ( ui->input_returnItem_lineEdit->text().isEmpty() ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error )
                     << "SwapDialog::returnItemSet called with empty returnItem" );
    }
    // Locate contract
    if ( locateContract( ui->input_returnItem_lineEdit->text() ) ) {
        log.stream() << "Contract found, with id '"
                << m_contract.getId()
                << "'";
        ui->output_contract_textBrowser->setText( m_contract.toReturnHtml() );
        // Lock fields and change focus to rent line edit.
        ui->input_returnItem_lineEdit->setReadOnly( true );
        ui->input_lookup_pushButton->setEnabled( false );
        ui->input_rentItem_lineEdit->setFocus();
    }
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ui->input_returnItem_lineEdit->isReadOnly() && !ui->input_rentItem_lineEdit->text().isEmpty() );
}

void SwapDialog::rentItemSet() {
    Logger log("void SwapDialog::rentItemSet()");
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ui->input_returnItem_lineEdit->isReadOnly() && !ui->input_rentItem_lineEdit->text().isEmpty() );
    if ( m_contract.getId() < 0 ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error )
                     << "Error: Wanting to swap in SwapDialog::rentItemSet, but no contract set... can't complete swap operation");
    }
    m_contract.swapItems( ui->input_returnItem_lineEdit->text(),
                          ui->input_rentItem_lineEdit->text() );
    // Print the contract.
    if ( Globals::checkPosPrinter() ) {
        Pos::Printer & posp( Globals::getPosPrinter() );
        posp.startReceipt();
        m_contract.printReturn( posp );
        posp.endReceipt();
        QMessageBox::information( this, tr("Items swapped"),
                                  tr( "Items swapped. Printing receipt." ) );
    } else {
        QMessageBox::information( this, tr( "Items swapped" ),
                                  tr( "Items swapped. Unable to print receipt." ) );
    }
    // Close the window
    close();
}

void SwapDialog::on_input_lookup_pushButton_clicked()
{
    Logger log("void SwapDialog::on_input_lookup_pushButton_clicked()");
    returnItemSet();
}

void SwapDialog::on_input_returnItem_lineEdit_textChanged(QString value )
{
    Logger log("void SwapDialog::on_input_returnItem_lineEdit_textChanged(QString )");
    ui->input_lookup_pushButton->setEnabled( !value.isEmpty() );
}

void SwapDialog::on_buttonBox_accepted()
{
    // QMessageBox::information(this, "Accepted", "Jeg er accepted");
}

void SwapDialog::on_input_rentItem_lineEdit_textChanged(QString value )
{
    Logger log("void SwapDialog::on_input_rentItem_lineEdit_textChanged(QString )");

    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ui->input_returnItem_lineEdit->isReadOnly() && !ui->input_rentItem_lineEdit->text().isEmpty() );
}

void SwapDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    Logger log("void SwapDialog::on_buttonBox_clicked(QAbstractButton* button)");
    if ( button == ui->buttonBox->button( QDialogButtonBox::Ok ) ) {
        log.stream() << "Ok clicked";
        rentItemSet();
    }
}
