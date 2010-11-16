#include "returndialog.h"
#include "ui_returndialog.h"

// Qt
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSet>
#include <QAbstractState>

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
    connect( Globals::interceptor, SIGNAL(barcodeScan(QString)), this, SLOT(return_item(QString)));
    // connect( Globals::interceptor, SIGNAL( magSwipe(DKSundhedskort)), this, SLOT( set_hirer(DKSundhedskort) ) );

    // Initialize the state machine
    QState * blank          = new QState();
    blank->setObjectName( "blank" );
    QState * has_contract     = new QState();
    has_contract->setObjectName( "has_contract" );

    m_state_machine.addState( blank );
    m_state_machine.addState( has_contract );

    // No property assignments yet, always enabled.
    blank->addTransition( this, SIGNAL( contract_found()), has_contract );
    blank->addTransition( this, SIGNAL( item_returned()), has_contract );

    //TODO: Back to blank, or something?
    // Set initial state and start
    m_state_machine.setInitialState( blank );
    log.stream() << "Starting state machine";
    m_state_machine.start();
    QCoreApplication::processEvents();

    // Make sure the UI is set correctly to start with
    showMaximized();
    update();

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
    // Update the item, and the contractitem
    m_contract.returnItem( item_id );
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


void ReturnDialog::on_input_returnItem_pushButton_clicked() {
    Logger log("void ReturnDialog::on_input_returnItem_pushButton_clicked()");
    return_item( ui->input_item_lineEdit->text() );
}
