#include "hirerdialog.h"
#include "ui_hirerdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QState>

// App
#include "log.h"
#include "exception.h"
#include "utility.h"
#include "hirer.h"

using namespace Log;

/** \class HirerDialog
  * \brief A dialog to select, create or edit a hirer
  *
  * This dialog can be used for several different operations.
  *
  * Select a hirer:
  * The user can select a hirer by clicking on the list of hirers in the system.
  *
  * Add/Create a hirer:
  * The user can add a hirer by clicking add, when the values of a the fields have some values.
  * After an add, it is as if the user had selected the created hirer.
  *
  * Edit/Update a hirer
  * The user can edit a hirer, by: selecting a hirer, updating fields, and pressing the update button.
  *
  * Closing the dialog is either by "cancel", which only cancels the dialog, or "use", which sets up
  * a hirer instance for retrival by the caller
  *
  * State of the dialog, can be expressed like this
  *
  * start: Initial state => blank

  * blank: No data, no hirer
  *   active_buttons: Search
  *   // +Search => blank
  *   +Input  => no_hirer
  *   +Select => hirer_selected

  * no_hirer: No hirer has been selected - edit fields may contain data.
  *   active_buttons: Search, Reset, Add
  *   // +Search => no_hirer
  *   +Input  => no_hirer
  *   +Select => hirer_selected
  *   +Reset  => blank
  *   +Add    => hirer_selected

  * hirer_selected: The input fields match a hirer, and the hirer has been selected.
  *   active buttons: Search, Reset, Use
  *   // +Search => hirer_selected
  *   +Input  => hirer_changed
  *   +Select => hirer_selected
  *   +Reset  => blank
  *   +Use    => stop_with_hirer

  * hirer_changed : A hirer has been selected, but one or more input fields have been changed
  *   active_buttons: Search, Reset, Add, Update
  *   // +Search => hirer_changed
  *   +Input  => hirer_changed
  *   +Select => hirer_selected
  *   +Reset  => blank
  *   +Add    => hirer_selected
  *   +Update => hirer_selected

  * stop_with_hirer

  * stop_without_hirer (all cancel actions goes here).

  */



HirerDialog::HirerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HirerDialog)
{
    Logger log( "HirerDialog::HirerDialog(QWidget *parent)");
    ui->setupUi(this);

    // Set up the form to have a splitter. This could probably be done better.
    top_widget = new QWidget( this );
    top_widget->setLayout( ui->layout_top );

    splitter = new QSplitter( this );
    splitter->setOrientation( Qt::Vertical );

    splitter->addWidget( top_widget );
    splitter->addWidget( ui->tableView_hirers );

    splitter->setStretchFactor( 0, 1 );
    splitter->setStretchFactor( 1, 10 );

    top_grid_layout = new QGridLayout( this );
    top_grid_layout->addWidget( splitter, 0, 0 );

    setLayout( top_grid_layout );

    // Fix up the query model to provide all hirers when we are first crated.
    model = new QSqlQueryModel;
    model->setQuery("SELECT id, memberId, firstName, lastName, ssn, streetAddress, zip, city, country, note FROM hirers order by firstname");
    model->setHeaderData(0, Qt::Horizontal, tr( "Id") );
    model->setHeaderData(1, Qt::Horizontal, tr( "MemberId") );
    model->setHeaderData(2, Qt::Horizontal, tr( "Name") );
    model->setHeaderData(3, Qt::Horizontal, tr( "Surname") );
    model->setHeaderData(4, Qt::Horizontal, tr( "SSN") );
    model->setHeaderData(5, Qt::Horizontal, tr( "Address") );
    model->setHeaderData(6, Qt::Horizontal, tr( "Zip") );
    model->setHeaderData(7, Qt::Horizontal, tr( "City") );
    model->setHeaderData(8, Qt::Horizontal, tr( "Country" ) );
    model->setHeaderData(9, Qt::Horizontal, tr( "Note" ) );

    // QTableView *view = new QTableView;
    ui->tableView_hirers->setModel(model);

    // Connect all edit changes to input_field_edited.
    connect( ui->lineEdit_firstName, SIGNAL(textEdited(QString)), this, SLOT(on_input_field_edited(QString)));
    connect( ui->lineEdit_lastName, SIGNAL(textEdited(QString)), this, SLOT(on_input_field_edited(QString)));
    connect( ui->lineEdit_streetAddress, SIGNAL(textEdited(QString)), this, SLOT(on_input_field_edited(QString)));
    connect( ui->lineEdit_zip, SIGNAL(textEdited(QString)), this, SLOT(on_input_field_edited(QString)));
    connect( ui->lineEdit_city, SIGNAL(textEdited(QString)), this, SLOT(on_input_field_edited(QString)));
    connect( ui->lineEdit_country, SIGNAL(textEdited(QString)), this, SLOT(on_input_field_edited(QString)));
    connect( ui->lineEdit_memberId, SIGNAL(textEdited(QString)), this, SLOT(on_input_field_edited(QString)));
    connect( ui->lineEdit_SSN, SIGNAL(textEdited(QString)), this, SLOT(on_input_field_edited(QString)));
    connect( ui->plainTextEdit_note, SIGNAL(textChanged()), this, SIGNAL(on_note_field_changed()));

    // Initialize the state machine
    QState * blank          = new QState();
    QState * no_hirer       = new QState();
    QState * hirer_selected = new QState();
    QState * hirer_changed  = new QState();

    state_machine.addState( blank );
    state_machine.addState( no_hirer );
    state_machine.addState( hirer_selected );
    state_machine.addState( hirer_changed );

    // Debug state changes - FIXME: Remove
    blank->assignProperty( ui->label_7, "text", "blank");
    no_hirer->assignProperty( ui->label_7, "text", "no_hirer");
    hirer_selected->assignProperty( ui->label_7, "text", "hirer_selected");
    hirer_changed->assignProperty( ui->label_7, "text", "hirer_changed");

    // BLANK
    blank->assignProperty( ui->pushButton_clear, "enabled", false );
    blank->assignProperty( ui->pushButton_add, "enabled", false );
    blank->assignProperty( ui->pushButton_update, "enabled", false );
    blank->assignProperty( ui->pushButton_use, "enabled", false );
    // From blank to no_hirer on any input.
    blank->addTransition( this, SIGNAL(input_field_edited()), no_hirer );
    blank->addTransition( this, SIGNAL( select_hirer() ), hirer_selected );

    // NO_HIRER
    // Add code to enable/disable buttons on state change.
    no_hirer->assignProperty( ui->pushButton_clear, "enabled", true );
    no_hirer->assignProperty( ui->pushButton_add, "enabled", true );
    no_hirer->assignProperty( ui->pushButton_update, "enabled", false );
    no_hirer->assignProperty( ui->pushButton_use, "enabled", false );
    no_hirer->addTransition( this, SIGNAL( select_hirer() ), hirer_selected );
    no_hirer->addTransition( ui->pushButton_clear, SIGNAL( clicked() ), blank );
    // No direct connection to this button; no_hirer->addTransition( ui->pushButton_add, SIGNAL(clicked()), hirer_selected );

    // HIRER_SELECTED
    hirer_selected->assignProperty( ui->pushButton_clear, "enabled", true );
    hirer_selected->assignProperty( ui->pushButton_add, "enabled", false );
    hirer_selected->assignProperty( ui->pushButton_update, "enabled", false );
    hirer_selected->assignProperty( ui->pushButton_use, "enabled", true );
    hirer_selected->addTransition( this, SIGNAL(input_field_edited()), hirer_changed);
    hirer_selected->addTransition( ui->pushButton_clear, SIGNAL( clicked() ), blank );
    // FIXME: Use to exit
    hirer_changed->assignProperty( ui->pushButton_clear, "enabled", true );
    hirer_changed->assignProperty( ui->pushButton_add, "enabled", true );
    hirer_changed->assignProperty( ui->pushButton_update, "enabled", true );
    hirer_changed->assignProperty( ui->pushButton_use, "enabled", false );
    hirer_changed->addTransition( this, SIGNAL( select_hirer() ), hirer_selected );
    hirer_changed->addTransition( ui->pushButton_clear, SIGNAL( clicked() ), blank );
    // Not directly to this button; hirer_changed->addTransition( ui->pushButton_add, SIGNAL(clicked()), hirer_selected );
    // Not directly to this button either; hirer_changed->addTransition( ui->pushButton_update, SIGNAL(clicked()), hirer_selected );

    // Set initial state and start
    state_machine.setInitialState( blank );
    state_machine.start();
    QCoreApplication::processEvents();

    // Connect the use button - which is only available when a hirer is selected - to the accept slot
    connect( ui->pushButton_use, SIGNAL( clicked()), this, SLOT( accept()) );
}

HirerDialog::~HirerDialog()
{
    Logger log( "HirerDialog::~HirerDialog()");
    delete ui;
}

Hirer HirerDialog::getHirer() {
    Logger log( "Hirer HirerDialog::getHirer()");
    // TODO Check if Hirer is valid
    return hirer;
}

// Any input fields changed, we get here.
// Check if we need auto lookup, perform if yes.
// Check if empty, go to blank state, if that is the case
// Otherwise, emit input_field_edited to state machine
void HirerDialog::on_input_field_edited(QString value) {
    Logger log( "HirerDialog::on_input_field_edited(const QString & value)");
    if ( ui->checkBox_autoLookup->isChecked() ) {
        updateQueryModel();
    }
    // Todo: If no values in all fields, we should go to blank state.
    emit input_field_edited();
}

// This is just because the note field does not have a signal like line edits.
void HirerDialog::on_note_field_changed() {
    Logger log( "void HirerDialog::on_note_field_changed()" );
    // on_input_field_edited ignores the string
    on_input_field_edited( ui->plainTextEdit_note->toPlainText());
}

// Qt boilerplate...
void HirerDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

// This method updates the query based on the values in the input fields, expect the value in the note field
// TODO: Enable search based on value of note ?
void HirerDialog::updateQueryModel() {
    Logger log( "void HirerDialog::updateQueryModel()" );
    // This may have to be changed, if we want to support automatic updating of the display.
    PROTECT_BLOCK(
        // Set up the query to retrieve all records, matching the current selections from the edit lines.
        QSqlQuery tmpQ;
        query_check_prepare( tmpQ, "select id, memberId, firstName, lastName, ssn, streetAddress, zip, city, country, note "
                                   "from hirers "
                                   "where memberId like :memberId "
                                   "and firstName like :firstName "
                                   "and lastName like :lastName "
                                   "and ssn like :ssn "
                                   "and streetAddress like :streetAddress "
                                   "and zip like :zip "
                                   "and city like :city "
                                   "and country like :country "
                                   "order by firstname, lastname, id" );
        tmpQ.bindValue( ":memberId",      QString ( "%%%1%%").arg( ui->lineEdit_memberId->text() ) );
        tmpQ.bindValue( ":firstName",     QString ( "%%%1%%").arg( ui->lineEdit_firstName->text() ) );
        tmpQ.bindValue( ":lastName",      QString ( "%%%1%%").arg( ui->lineEdit_lastName->text() ) );
        tmpQ.bindValue( ":ssn",           QString ( "%%%1%%").arg( ui->lineEdit_SSN->text() ) );
        tmpQ.bindValue( ":streetAddress", QString ( "%%%1%%").arg( ui->lineEdit_streetAddress->text() ) );
        tmpQ.bindValue( ":zip",           QString ( "%%%1%%").arg( ui->lineEdit_zip->text() ) );
        tmpQ.bindValue( ":city",          QString ( "%%%1%%").arg( ui->lineEdit_city->text() ) );
        tmpQ.bindValue( ":country",       QString ( "%%%1%%").arg( ui->lineEdit_country->text() ) );
        query_check_exec( tmpQ );

        // When setting the model, the UI will be updated
        model->setQuery( tmpQ );
    );
}

// This loads a hirer from the database into our hirer.
// Id is the id in the database.
void HirerDialog::loadHirer( qlonglong id ) {
    Logger log( "void HirerDialog::loadHirer( qlonglong id )" );
    PROTECT_BLOCK(
       Hirer hirer( id );
       ui->lineEdit_memberId->setText( hirer.m_memberId );
       ui->lineEdit_firstName->setText( hirer.m_firstName );
       ui->lineEdit_lastName->setText( hirer.m_lastName );
       ui->lineEdit_SSN->setText( hirer.m_ssn );
       ui->lineEdit_streetAddress->setText( hirer.m_streetAddress );
       ui->lineEdit_zip->setText( hirer.m_zip );
       ui->lineEdit_city->setText( hirer.m_city );
       ui->lineEdit_country->setText( hirer.m_country );
       ui->plainTextEdit_note->setPlainText( hirer.m_note );
       this->hirer = hirer;
       emit select_hirer();
       updateQueryModel();
    );
}
void HirerDialog::on_pushButton_lookup_clicked() {
    Logger log( "void HirerDialog::on_pushButton_lookup_clicked()" );
    updateQueryModel();
}

// Translate from the models index to the actual index of the hirer in the database,
// then load the hirer from the index.
void HirerDialog::on_tableView_hirers_doubleClicked(QModelIndex index) {
    Logger log( "void HirerDialog::on_tableView_hirers_doubleClicked(QModelIndex index)" );
    // Get the data from the model, then load a hirer
    // Note, this assumes that column 0 is always the id. It must be...
    QModelIndex myIndex = model->index( index.row(), 0 );
    qlonglong id = model->data( myIndex ).toLongLong();
    loadHirer( id );
}

// Clear the dialog.
void HirerDialog::on_pushButton_clear_clicked() {
    Logger log( "void HirerDialog::on_pushButton_clear_clicked()" );
    ui->lineEdit_memberId->setText( "" );
    ui->lineEdit_firstName->setText( "" );
    ui->lineEdit_lastName->setText( "" );
    ui->lineEdit_SSN->setText( "" );
    ui->lineEdit_streetAddress->setText( "" );
    ui->lineEdit_zip->setText( "" );
    ui->lineEdit_city->setText( "" );
    ui->lineEdit_country->setText( "" );
    ui->plainTextEdit_note->setPlainText( "" );
    ui->lineEdit_firstName->setFocus();
    updateQueryModel();
}

void HirerDialog::on_pushButton_add_clicked()
{
    // TODO: If the values reflecting the current input fields, can be added to the hirer, then
    // emit select_hirer()
}

void HirerDialog::on_pushButton_update_clicked()
{
    // TODO: As for add, more or less
    // emit select_hirer();
}

// If auto search is changed, enable/disable manual search.
void HirerDialog::on_checkBox_autoLookup_stateChanged(int newState ) {
    ui->pushButton_lookup->setEnabled( newState == Qt::Unchecked );
    if ( Qt::Checked == newState ) {
        updateQueryModel();
    }
}
