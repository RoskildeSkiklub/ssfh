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

#include "returndialog.h"
#include "ui_returndialog.h"

// Qt
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSet>
#include <QAbstractState>
#include <QCloseEvent>

// app
#include "globals.h"
#include "log.h"
#include "exception.h"
#include "utility.h"
#include "item.h"
#include "db_consts.h"

using namespace Log;

/*
 *
 * State of the dialog can be expressed like this
 *
 * start: Initial state => blank
 *
 * blank: No contract
 *   active buttons: find contract, return item, done
 *   contract_found => has_contract
 *   item_returned => has_contract
 *   contract_closed => invalid

 * has_contract: Contract is set
 *   active buttons: return item, done
 *   item_returned => self/ignored
 *   contract_closed => blank??? */

ReturnDialog::ReturnDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReturnDialog)
{
    Logger log("ReturnDialog::ReturnDialog(QWidget *parent)");
    ui->setupUi(this);

    // Get the signals from the scan input events.
    connect( Globals::interceptor, SIGNAL(barcodeItemScan(QString)), this, SLOT(return_item(QString)));
    // connect( Globals::interceptor, SIGNAL( magSwipe(DKSundhedskort)), this, SLOT( set_hirer(DKSundhedskort) ) );

    // Initialize the state machine
    QState * blank          = new QState();
    blank->setObjectName( "blank" );
    QState * has_contract     = new QState();
    has_contract->setObjectName( "has_contract" );

    m_state_machine.addState( blank );
    m_state_machine.addState( has_contract );


    blank->assignProperty( ui->input_returnAll_pushButton, "enabled", false );
    blank->addTransition( this, SIGNAL( contract_found()), has_contract );
    blank->addTransition( this, SIGNAL( item_returned()), has_contract );

    has_contract->assignProperty( ui->input_returnAll_pushButton, "enabled", true );

    //TODO: Back to blank, or something?
    // Set initial state and start
    m_state_machine.setInitialState( blank );
    log.stream() << "Starting state machine";
    m_state_machine.start();
    QCoreApplication::processEvents();

    // Make sure the UI is set correctly to start with
    showMaximized();
    update();

    // Provide audio feedback when returning items
    connect(this, SIGNAL(item_returned()),
            Globals::getFeedbackObject(), SLOT(itemReturned()));

}

void ReturnDialog::update() {
    Logger log("void ReturnDialog::update()");
    if ( is_in_state( "blank" ) ) {
        ui->output_contract_textBrowser->setText( tr("<p>Select a contract or scan an item</p>") );
    } else {
        ui->output_contract_textBrowser->setText( m_contract.toReturnHtml() );
    }
}

bool ReturnDialog::is_in_state( const QString & state ) {
    Logger log( "bool ReturnDialog::is_in_state( const QString & state )" );
    log.stream() << "Checking state '" << state << "'";
    if ( get_current_states( m_state_machine ).contains( state ) ) {
        log.stream() << "Found state, returning true";
        return true;
    } else {
        log.stream() << "Could not find state, returning false";
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////
// ITEM HANDLING
void ReturnDialog::return_item(const QString &item_id) {
    //! \todo BETTER ERROR HANDLING.
    Logger log("void ReturnDialog::return_item(const QString &item_id)");
    log.stream() << "Trying to return item with id '" << item_id << "'";
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
        return;
    }
    log.stream() << "Item loaded, and is out. Checking for contract.";
    // If we have a contract - make sure this is the same contract
    if ( is_in_state( "blank") ) {
        log.stream() << "In blank state. Locate the contract and use it";
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
                return;

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
        m_contract = contract;
        emit contract_found();
        update();
    } else {
        log.stream() << "Already have contract with id '" << m_contract.getId();
        // Do nothing, really. A contract is present.
    }
    // Update the item, and the contractitem - this may fail - translate common errors to the user
    try {
        m_contract.returnItem( item_id );
    } catch ( const Exception & e ) {
        log.stream( info ) << "Got exception from Contract::returnItem : '" << e.toString() << "'";
        switch ( e.getStatusCode() ) {
        case Errors::ItemNotPartOfContract: {
                QMessageBox::critical( this, tr("Item not part of contract" ),
                                       tr( "The item is not part of this contract. Unable to return it at this point." ) );
                update();
                return;
            }
        default: throw;
        }
    }
    emit item_returned();
    update();
    if ( ! m_contract.hasReturnableItems() ) {
        m_contract.close();
        if ( Globals::checkPosPrinter() ) {
            Pos::Printer & posp( Globals::getPosPrinter() );
            posp.startReceipt();
            m_contract.printReturn( posp );
            posp.endReceipt();
            QMessageBox::information( this, tr("Contract closed"),
                                      tr( "All items returned. Printing receipt." ) );
        } else {
            QMessageBox::information( this, tr( "Contract closed" ),
                                      tr( "All items returned. Unable to print receipt." ) );
        }
        close();
    }
}


////////////////////////////////////////////////////////////////////////////


ReturnDialog::~ReturnDialog()
{
    delete ui;
}

void ReturnDialog::closeEvent(QCloseEvent * event) {
    Logger log("void ReturnDialog::closeEvent(QCloseEvent *)");
    // If contract is closed, or "blank", just close. Otherwise, ask.
    if ( is_in_state( "blank" ) ) {
        event->accept();
        return;
    }
    if ( m_contract.getState() == DB::Contract::State::closed ) {
        event->accept();
        return;
    }
    // Not blank, not closed, lets ask
    if ( QMessageBox::Yes
         == QMessageBox::question( this,
                                   tr( "Are you sure you want to close?"),
                                   tr( "Contract is not closed, and may have returnable items. Are you sure you wish to close?" ),
                                   QMessageBox::Yes | QMessageBox::No ) ) {
        event->accept();
        return;
    } else {
        event->ignore();
        return;
    }
}


void ReturnDialog::on_input_returnItem_pushButton_clicked() {
    Logger log("void ReturnDialog::on_input_returnItem_pushButton_clicked()");
    return_item( ui->input_item_lineEdit->text() );
}

void ReturnDialog::on_input_done_pushButton_clicked()
{
    Logger log("void ReturnDialog::on_input_done_pushButton_clicked(");
    close();
}

void ReturnDialog::on_input_returnAll_pushButton_clicked()
{
    Logger log("void ReturnDialog::on_input_returnAll_pushButton_clicked()");
    if (  is_in_state( "has_contract" ) &&  QMessageBox::Yes
          == QMessageBox::question( this,
                                    tr("Are you sure you want to return all items"),
                                    tr( "Are you sure you want to return all items on this contract at once, without returning individual items?" ),
                                    QMessageBox::Yes | QMessageBox::No ) ) {
        log.stream() << "Returning all items";
        m_contract.returnAll();
        update();
        if ( ! m_contract.hasReturnableItems() ) {
            m_contract.close();
            if ( Globals::checkPosPrinter() ) {
                Pos::Printer & posp( Globals::getPosPrinter() );
                posp.startReceipt();
                m_contract.printReturn( posp );
                posp.endReceipt();
                QMessageBox::information( this, tr("Contract closed"),
                                          tr( "All items returned. Printing receipt." ) );
            } else {
                QMessageBox::information( this, tr( "Contract closed" ),
                                          tr( "All items returned. Unable to print receipt." ) );
            }
            close();
        } else {
            throw Exception( Errors::InternalError)
                    << ( log.stream( error )
                         << "Returned all items, but contract still has returnable items"
                         );
        }
    }
}
