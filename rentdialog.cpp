#include "rentdialog.h"
#include "ui_rentdialog.h"

// C++
#include <typeinfo>

// Qt
#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSet>
#include <QAbstractState>
#include <QDialog>
#include <QCloseEvent>

// App
#include "globals.h"
#include "log.h"
#include "exception.h"
#include "hirerdialog.h"
#include "dksundhedskort.h"
#include "utility.h"
#include "plaintexteditdialog.h"
#include "db_consts.h"

using namespace Log;


/*

* State of the dialog, can be expressed like this
*
* start: Initial state => blank

* blank: No hirer, no duration, no items
*   active_buttons: Add/search for hirer, Cancel
*   hirer_set => has_hirer (can retrive parked contract, which can be in another state!).

* has_hirer: Hirer has been selected, duration and items missing
*   active_buttons: Change duration, Add item, Park, Cancel
*   duration_changed => has_hirer
*   +cancel => revert contract
*   item_added => has_item

* has_item: Hirer has been selected, duration is fixed, at least one item is added
*   active_buttons: Rentalgroup, Add item, Finish, Park, Cancel, Edit note
*   +rental group  => has_item
*   +item_added => has_item
*   +cancel => revert contract
*   +park => new contract
*   +finish => Print n' stuff. new contract

*/

RentDialog::RentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RentDialog)
{
    Logger log( "RentDialog::RentDialog(QWidget *parent)");
    ui->setupUi(this);
    // TODO: Control on this... Can't remember what I wanted to do with this, really.....
    // log.stream() << "Starting transaction";
/*    if ( ! QSqlDatabase::database().transaction() ) {
        log.stream( error ) << "Unable to start transaction";
    } */
    //connect( ui->addButton, SIGNAL(clicked()), this, SLOT(on_addButton_clicked()));

    // Get the signals from the scan input events.
    connect( Globals::interceptor, SIGNAL( barcodeItemScan(QString)), this, SLOT( add_item(QString)));
    connect( Globals::interceptor, SIGNAL( barcodeCommandScan(Globals::BarcodeCommands::Command)),
             this, SLOT( on_barcodeCommandScan(Globals::BarcodeCommands::Command) ) );
    connect( Globals::interceptor, SIGNAL( magSwipe(DKSundhedskort)), this, SLOT( set_hirer(DKSundhedskort) ) );

    // Initialize the state machine
    QState * blank          = new QState();
    blank->setObjectName( "blank" );
    QState * has_hirer      = new QState();
    has_hirer->setObjectName( "has_hirer" );
    QState * has_item       = new QState();
    has_item->setObjectName( "has_item" );

    m_state_machine.addState( blank );
    m_state_machine.addState( has_hirer );
    m_state_machine.addState( has_item );

    // BLANK
    blank->assignProperty( ui->input_addItem_pushButton, "enabled", false );
    blank->assignProperty( ui->input_duration_pushButton, "enabled", false );
    blank->assignProperty( ui->input_finish_pushButton, "enabled", false );
    blank->assignProperty( ui->input_hirer_pushButton, "enabled", true );
    blank->assignProperty( ui->input_park_pushButton, "enabled", false );
    blank->assignProperty( ui->input_item_lineEdit, "enabled", false );
    blank->assignProperty( ui->input_rentalgroup_pushButton, "enabled", false );
    blank->assignProperty( ui->input_editNote_pushButton, "enabled", false );
    // From blank to has_hirer if hirer_set
    blank->addTransition( this, SIGNAL( hirer_set() ), has_hirer );

    // HAS_HIRER
    has_hirer->assignProperty( ui->input_addItem_pushButton, "enabled", !ui->input_item_lineEdit->text().isEmpty() );
    has_hirer->assignProperty( ui->input_duration_pushButton, "enabled", true );
    has_hirer->assignProperty( ui->input_finish_pushButton, "enabled", false );
    has_hirer->assignProperty( ui->input_hirer_pushButton, "enabled", false );
    has_hirer->assignProperty( ui->input_park_pushButton, "enabled", true );
    has_hirer->assignProperty( ui->input_item_lineEdit, "enabled", true );
    has_hirer->assignProperty( ui->input_rentalgroup_pushButton, "enabled", false );
    has_hirer->assignProperty( ui->input_editNote_pushButton, "enabled", false );
    // From has_hirer to has_item on item_added
    has_hirer->addTransition( this, SIGNAL( item_added() ), has_item );
    has_hirer->addTransition( this, SIGNAL( contract_blanked()), blank );
    // TODO: TO BLANK

    // has_item
    has_item->assignProperty( ui->input_addItem_pushButton, "enabled", !ui->input_item_lineEdit->text().isEmpty() );
    has_item->assignProperty( ui->input_duration_pushButton, "enabled", false );
    has_item->assignProperty( ui->input_finish_pushButton, "enabled", true );
    has_item->assignProperty( ui->input_hirer_pushButton, "enabled", false );
    has_item->assignProperty( ui->input_park_pushButton, "enabled", true );
    has_item->assignProperty( ui->input_item_lineEdit, "enabled", true );
    has_item->assignProperty( ui->input_rentalgroup_pushButton, "enabled", true );
    has_item->assignProperty( ui->input_editNote_pushButton, "enabled", true );
    // TODO: From has_item to no_hirer on item_added ===> BLANK...
    has_item->addTransition( this, SIGNAL( contract_blanked()), blank );
    // blank->addTransition( this, SIGNAL( item_added()) ), has_item );

    // Set initial state and start
    m_state_machine.setInitialState( blank );
    m_state_machine.start();
    QCoreApplication::processEvents();

    // Make sure the UI is set correctly to start with
    showMaximized();
    update();
}

RentDialog::~RentDialog()
{
    delete ui;
}

void RentDialog::useParkedContract( qlonglong contract_id) {
    Logger log("RentDialog::useParkedContract(const QString &contract_id)");
    if ( !is_in_state( "blank" ) ) {
        throw Exception( Errors::InternalError )
                << ( log.stream(error) << "Dialog not blank, when useParkedContract was called" );
    }
    m_contract = Contract::db_load( contract_id );
    m_contract.db_unPark();
    // The hirer is set, or the contract could not be parked
    emit hirer_set();
    // If the contract has items - then emit item_added
    if ( m_contract.hasItems() ) {
        log.stream() << "Contract has items";
        emit item_added();
    }
    update();
}

void RentDialog::closeEvent(QCloseEvent * event) {
    Logger log("RentDialog::closeEvent(QCloseEvent *)");
    // If blank, just close
    // If has_hirer, ask if sure to close
    // If has_item, ask if parking or cancel
    if ( is_in_state( "blank" ) ) {
        event->accept();
        return;
    }
    if ( is_in_state( "has_hirer" ) ) {
        if ( QMessageBox::Yes
             == QMessageBox::question( this,
                                       tr("Cancel Contract?"),
                                       tr( "Are you sure you want to cancel the contract?" ),
                                       QMessageBox::Yes | QMessageBox::No ) ) {
            event->accept();
            return;
        } else {
            event->ignore();
            return;
        }
    }
    if ( is_in_state( "has_item") && m_contract.getState() != DB::Contract::State::active ) {
        log.stream( info ) << "Close called, but dialog is in has_item state and contract is not active. Asking user what to do";
        QMessageBox mbox( QMessageBox::Warning, tr( "Contract has not been activated"),
                          tr("The contract must be cancelled, parked or activated in order to close this window.<br>"
                             "Would you like to..."), QMessageBox::Cancel );
        QPushButton * cancelContract = mbox.addButton( tr("Cancel Contract"), QMessageBox::DestructiveRole );
        QPushButton * parkContract = mbox.addButton( tr( "Park Contract"), QMessageBox::ActionRole );
        QPushButton * activateContract = mbox.addButton( tr( "Activate Contract"), QMessageBox::ActionRole );
        mbox.exec();
        if( QMessageBox::Cancel == mbox.result() ) {
            log.stream( info ) << "User choose to cancel close";
            event->ignore();
            return;
        }
        if ( cancelContract == mbox.clickedButton() ) {
            log.stream( info ) << "User choose to cancel the contract";
            m_contract.cancel();
            event->accept();
            return;
        }
        if ( parkContract == mbox.clickedButton() ) {
            log.stream( info ) << "User choose to park the contract";
            m_contract.park();
            event->accept();
            return;
        }
        if ( activateContract == mbox.clickedButton() ) {
            log.stream( info ) << "User choose to activate the contract";
            finish();
            event->accept();
            return;
        }
        //throw Exception( Errors::InternalError )
        log.stream(error) << "Internal error in RentDialog::closeEvent, value returned from user question was not handled";
        event->accept();
        return;
    }
    log.stream(error) << "Internal error in RentDialog::closeEvent, missing handler for dialog state";
    event->accept();
    return;
}

void RentDialog::on_barcodeCommandScan(const Globals::BarcodeCommands::Command &command) {
    Logger log("void RentDialog::on_barcodeCommandScan(const Globals::BarcodeCommands::Command &command)");
    if ( QApplication::activeWindow() != this ) {
        log.stream() << "Ignoring commandScan because window is not active";
        return;
    }
    if ( command.m_code == Globals::BarcodeCommands::OperationDone ) {
        log.stream() << "Command is 'OperationDone' - calling finish";
        finish();
    }
}



void RentDialog::update() {
    Logger log( "void RentDialog::update()" );
    ui->output_contract_textBrowser->setText( m_contract.toRentalHtml() );
}

bool RentDialog::is_in_state( const QString & state ) {
    Logger log( "bool RentDialog::is_in_state( const QString & state )" );
    log.stream() << "Checking state '" << state << "'";
    if ( get_current_states( m_state_machine ).contains( state ) ) {
        log.stream() << "Found state, returning true";
        return true;
    } else {
        log.stream() << "Could not find state, returning false";
        return false;
    }
}

void RentDialog::finish() {
    Logger log("void RentDialog::finish()");
    if ( is_in_state( "has_item" ) ) {
        m_contract.activate();
        if ( Globals::checkPosPrinter() ) {
            Pos::Printer & posp( Globals::getPosPrinter() );
            // The receipt the customer needs to sign
            posp.startReceipt();
            m_contract.printRental( posp );
            posp.endReceipt();

            // The receipt for the customer
            posp.startReceipt();
            m_contract.printReceipt( posp );
            posp.endReceipt();
            QMessageBox::information( this, tr( "Contract created" ),
                                      tr( "Contract created, printing receipt." ) );
        } else {
            QMessageBox::information( this, tr( "Contract created" ),
                                      tr( "Contract created. Unable to print receipt." ) );
        }
        close();
    } else {
        log.stream( warn ) << "Ignoring call to finish, because state is not has_item";
    }
}

////////////////////////////////////////////////////////////////////////////////
// HIRER SETTING AND OTHER RELATED METHODS
void RentDialog::set_hirer ( const Hirer & hirer ) {
    Logger log( "void RentDialog::set_hirer (const Hirer &dsk)" );
    // Check state
    if ( ! is_in_state( "blank" ) ) {
        log.stream( error ) << "State is not blank - a hirer has already been set for this contract";
        QMessageBox::critical( this, tr( "Hirer already set" ), tr( "A hirer has already been set for this contract.") );
        return;
    }
    if ( m_contract.setHirer( hirer ) ) {
        log.stream() << "Hirer set, emitting hirer_set signal";
        emit hirer_set();
        update();
    } else {
        log.stream() << "Contract did not wish to set the hirer - is this a problem?";
        // TODO: May actually want an exception from Contract::setHirer... argh.
        QMessageBox::warning( this, tr( "Could not set hirer"), tr( "Could not set hirer for contract" ) );
    }
}

////////////////////////////////////////////////////////////////////////////////
// ITEM RELATED STUFF

// Can be used as target for barcode scanner signals.
void RentDialog::add_item( const QString & item_id ) {
    Logger log( "void RentDialog::add_item( const QString & item_id )" );
    if ( try_add_item( item_id ) ) {
        ui->input_item_lineEdit->setText("");
        emit item_added();
        update();
    }
}

bool RentDialog::try_add_item(const QString &item_id) {
    Logger log( "bool RentDialog::try_add_item(const QString &item_id)" );
    // This may have to be changed, if/when we want to react smarter to errors such as duplicating entries.
    try {
        m_contract.addItem( item_id );
    }
    catch( const Exception & e ) {
        log.stream( info ) << "Got exception from Contract::add_item : '" << e.toString() << "'";
        switch ( e.getStatusCode() ) {
        case Errors::ItemAlreadyPartOfContract: {
            //TODO: ask the user if remove...
            return false; // Perhaps something else, need to check if no more items, stuff like that.
        }
        case Errors::ItemDoesNotExist: {
            QMessageBox::warning( this, tr( "Item not registered in database" ),
                                  tr( "Item with id '%0' was not found in database. Unable to add to contract.").arg(item_id) );
            // TODO: Should we be able to add the new item here?
            return false;
        }
        case Errors::ItemUnavailable: {
            QMessageBox::warning( this, tr( "The item is unavailable"),
                                  tr( "The item with id '%0' is already registered on another contract. Unable to add to contract.").arg(item_id) );
            // TODO: REALLY should be able to mark returned, and add to this contract. Real life trumps electronic.
            return false;
        }
        default: throw;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// SLOTS

void RentDialog::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void RentDialog::set_hirer (const DKSundhedskort &dsk) {
    Logger log( "void RentDialog::set_hirer (const DKSundhedskort &dsk)" );
    Hirer hirer( dsk.name, dsk.surname, dsk.address, dsk.zip, dsk.city, dsk.country, dsk.ssn );
    set_hirer( hirer );
}

void RentDialog::on_input_hirer_pushButton_clicked() {
    Logger log( "void RentDialog::on_input_hirer_pushButton_clicked()" );
    log.stream() << "Displaying HirerDialog";
    HirerDialog hirerDialog;
    if ( QDialog::Accepted == hirerDialog.exec() ) {
        log.stream() << "Getting hirer from hirerDialog";
        log.stream() << "Hirer is " << hirerDialog.getHirer().toHtml();
        set_hirer( hirerDialog.getHirer() );
    } else {
        log.stream() << "Did not get any hirer from hirerDialog";
    }
}

void RentDialog::on_input_addItem_pushButton_clicked() {
    Logger log( "void RentDialog::on_input_addItem_pushButton_clicked()" );
    add_item( ui->input_item_lineEdit->text() );
}

void RentDialog::on_input_item_lineEdit_textChanged(QString newText) {
    ui->input_addItem_pushButton->setEnabled( ( ! newText.isEmpty() )
                                              && ( is_in_state( "has_hirer")
                                                   || is_in_state( "has_item" ) ) );
}

void RentDialog::on_input_finish_pushButton_clicked() {
    Logger log("void RentDialog::on_input_finish_pushButton_clicked()");
    finish();
}

void RentDialog::on_input_editNote_pushButton_clicked() {
    Logger log("void RentDialog::on_input_editNote_pushButton_clicked()");
    PlainTextEditDialog pted( tr( "Set note for contract"),
                              tr( "<b>Current note</b>"),
                              m_contract.getNote() );
    if ( QDialog::Accepted == pted.exec()) {
        log.stream() << "Setting note on contract.";
        m_contract.setNote( pted.getText() );
        update();
    }
}

void RentDialog::on_input_cancel_pushButton_clicked() {
    Logger log("void RentDialog::on_input_cancel_pushButton_clicked()");
    if ( QMessageBox::Yes
         == QMessageBox::question( this,
                                   tr("Cancel Contract?"),
                                   tr( "Are you sure you want to cancel the contract?" ),
                                   QMessageBox::Yes | QMessageBox::No ) ) {
        m_contract.cancel(); // Explicit cancel
        emit contract_blanked(); // To make sure we can close the window
        close();
    }
}

void RentDialog::on_input_park_pushButton_clicked() {
    Logger log("void RentDialog::on_input_park_pushButton_clicked()");
    if ( QMessageBox::Yes
         == QMessageBox::question( this,
                                   tr("Park Contract?"),
                                   tr( "Are you sure you want to park the contract?" ),
                                   QMessageBox::Yes | QMessageBox::No ) ) {
        m_contract.park(); // Explicit cancel
        emit contract_blanked(); // To make sure we can close the window
        close();
    }
}
