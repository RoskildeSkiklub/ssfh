#include "rentdialog.h"
#include "ui_rentdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSet>
#include <QAbstractState>

// App
#include "globals.h"
#include "log.h"
#include "exception.h"
#include "hirerdialog.h"
#include "dksundhedskort.h"

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
*   active_buttons: Rentalgroup, Add item, Finish, Park, Cancel
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
    log.stream() << "Starting transaction";
    // TODO: Control on this... Can't remember what I wanted to do with this, really.....
/*    if ( ! QSqlDatabase::database().transaction() ) {
        log.stream( error ) << "Unable to start transaction";
    } */
    //connect( ui->addButton, SIGNAL(clicked()), this, SLOT(on_addButton_clicked()));

    // Get the signals from the scan input events.
    connect( Globals::interceptor, SIGNAL(barcodeScan(QString)), this, SLOT(add_item(QString)));
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

    // Debug state changes - FIXME: Remove
    /*blank->assignProperty( ui->label_7, "text", "blank");
    no_hirer->assignProperty( ui->label_7, "text", "no_hirer");
    hirer_selected->assignProperty( ui->label_7, "text", "hirer_selected");
    hirer_changed->assignProperty( ui->label_7, "text", "hirer_changed"); */

    // BLANK
    blank->assignProperty( ui->input_addItem_pushButton, "enabled", false );
    blank->assignProperty( ui->input_duration_pushButton, "enabled", false );
    blank->assignProperty( ui->input_finish_pushButton, "enabled", false );
    blank->assignProperty( ui->input_hirer_pushButton, "enabled", true );
    blank->assignProperty( ui->input_park_pushButton, "enabled", false );
    blank->assignProperty( ui->input_item_lineEdit, "enabled", false );
    blank->assignProperty( ui->input_rentalgroup_pushButton, "enabled", false );
    // From blank to has_hirer if hirer_set
    blank->addTransition( this, SIGNAL( hirer_set() ), has_hirer );

    // HAS_HIRER
    has_hirer->assignProperty( ui->input_addItem_pushButton, "enabled", true );
    has_hirer->assignProperty( ui->input_duration_pushButton, "enabled", true );
    has_hirer->assignProperty( ui->input_finish_pushButton, "enabled", false );
    has_hirer->assignProperty( ui->input_hirer_pushButton, "enabled", false );
    has_hirer->assignProperty( ui->input_park_pushButton, "enabled", true );
    has_hirer->assignProperty( ui->input_item_lineEdit, "enabled", true );
    has_hirer->assignProperty( ui->input_rentalgroup_pushButton, "enabled", false );
    // From has_hirer to has_item on item_added
    has_hirer->addTransition( this, SIGNAL( item_added() ), has_item );
    // TODO: TO BLANK

    // has_item
    has_item->assignProperty( ui->input_addItem_pushButton, "enabled", true );
    has_item->assignProperty( ui->input_duration_pushButton, "enabled", false );
    has_item->assignProperty( ui->input_finish_pushButton, "enabled", true );
    has_item->assignProperty( ui->input_hirer_pushButton, "enabled", false );
    has_item->assignProperty( ui->input_park_pushButton, "enabled", true );
    has_item->assignProperty( ui->input_item_lineEdit, "enabled", true );
    has_item->assignProperty( ui->input_rentalgroup_pushButton, "enabled", true );
    // TODO: From has_item to no_hirer on item_added ===> BLANK...
    // blank->addTransition( this, SIGNAL( item_added()) ), has_item );

    // Set initial state and start
    m_state_machine.setInitialState( blank );
    m_state_machine.start();

    // Make sure the UI is set correctly to start with
    update();
}

RentDialog::~RentDialog()
{
    delete ui;
}

void RentDialog::update() {
    Logger log( "void RentDialog::update()" );
    ui->output_contract_textEdit->setText( m_contract.toHtml() );
}

bool RentDialog::is_in_state( const QString & state ) {
    Logger log( "bool RentDialog::is_in_state( const QString & state )" );
    log.stream() << "Checking state '" << state << "'";
    QSet<QAbstractState *> states = m_state_machine.configuration();
    QAbstractState * statep;
    foreach( statep, states ) {
        if ( statep->objectName() == state ) {
            log.stream() << "Found it, returning true";
            return true;
        }
    }
    log.stream() << "Could not find it, returning false";
    return false;
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
        ui->output_contract_textEdit->setText( m_contract.toHtml() );
    }
}

bool RentDialog::try_add_item(const QString &item_id) {
    Logger log( "bool RentDialog::try_add_item(const QString &item_id)" );
    // This may have to be changed, if/when we want to react smarter to errors such as duplicating entries.
    // PROTECT_BLOCK(
            m_contract.add_item( item_id );
            return true;
            // );
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// SLOTS

void RentDialog::on_addButton_clicked() {
    Logger log( "void RentDialog::on_addButton_clicked()" );
    add_item( ui->input_item_lineEdit->text() );
}

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
    // TODO: Create a hirer from a suitable hirer constructor....
    // TODO: Call set_hirer with the new hirer
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
    // TODO: Read values back, etc.
}
