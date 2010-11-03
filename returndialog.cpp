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
    connect( Globals::interceptor, SIGNAL(barcodeScan(QString)), this, SLOT(add_item(QString)));
    connect( Globals::interceptor, SIGNAL( magSwipe(DKSundhedskort)), this, SLOT( set_hirer(DKSundhedskort) ) );

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
    update();

}

void ReturnDialog::update() {
    Logger log("void ReturnDialog::update()");
    if ( is_in_state( "blank" ) ) {
        ui->output_contract_textBrowser->setText( tr("<p>Select a contract or scan an item</p>") );
    } else {
        ui->output_contract_textBrowser->setText( m_contract.toHtml() );
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


ReturnDialog::~ReturnDialog()
{
    delete ui;
}
