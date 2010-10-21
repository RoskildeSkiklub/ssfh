#include "addnewitemdialog.h"
#include "ui_addnewitemdialog.h"

// Qt
#include <QSqlQuery>
#include <QIntValidator>
#include <QDateTime>

// App
#include "db_consts.h"
#include "globals.h"
#include "log.h"
#include "exception.h"
#include "utility.h"
#include "item.h"

using namespace Log;

AddNewItemDialog::AddNewItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewItemDialog)
{
    Logger log( "AddNewItemDialog::AddNewItemDialog(QWidget *parent)" );
    ui->setupUi(this);

    // Set validator for price thingy
    ui->input_price_lineEdit->setValidator(new QIntValidator(0,1000000, this) );

    log.stream() << "Filling in values in comboboxes";
    // PROTECT_BLOCK(
    QSqlQuery query;

    log.stream() << "Filling in values in type input";
    // TODO: Sort by number present in database.
    query_check_prepare( query, "select id from itemtypes order by id" );
    query_check_exec( query );
    query_check_first( query );
    do {
        ui->input_type_comboBox->addItem( query.value(0).toString() );
    } while ( query.next() );
    // Setup condition and rental group
    query_check_prepare( query, "select distinct condition from items order by condition" );
    query_check_exec( query );
    while( query.next() ) {
        ui->input_condition_comboBox->addItem( query.value(0).toString() );
    }
    query_check_prepare( query, "select distinct id from rentalgroups order by id" );
    query_check_exec( query );
    while( query.next() ) {
        ui->input_rentalgroup_comboBox->addItem( query.value(0).toString() );
    }

    // Set up mark and model, based on type.
    doSetupBasedOnType();

    // Connect signal from barcode to here...
    connect( Globals::interceptor, SIGNAL(barcodeScan(QString)), this, SLOT(add_item(QString)));

    //);
}

AddNewItemDialog::~AddNewItemDialog()
{
    delete ui;
}

void AddNewItemDialog::doSetupBasedOnType() {
    Logger log("void AddNewItemDialog::doSetupBasedOnType()");
    QSqlQuery query;

    // MARK
    log.stream() << "Setting up Mark";
    query_check_prepare( query, "select mark, count(*) from items where type=:type group by mark order by count(*) desc " );
    query.bindValue( ":type", ui->input_type_comboBox->currentText() );
    query_check_exec( query );
    // No first check - may be empty. Note, if not, ordered such that the mark with most is highlighted.
    // query_check_first( query );
    ui->input_mark_comboBox->clear();
    while ( query.next() ) {
        ui->input_mark_comboBox->addItem( query.value(0).toString() );
    }
    // ui->input_mark_comboBox->addItem( "" );

    // Cascade
    doSetupBasedOnMark();
}
void AddNewItemDialog::doSetupBasedOnMark() {
    Logger log("void AddNewItemDialog::doSetupBasedOnMark()");

    // MODEL
    log.stream() << "Setting up Model";
    QSqlQuery query;
    query_check_prepare( query, "select model, count(*) from items where type=:type and mark=:mark group by model order by count(*) desc " );
    query.bindValue( ":type", ui->input_type_comboBox->currentText() );
    query.bindValue( ":mark", ui->input_mark_comboBox->currentText() );
    query_check_exec( query );
    // No first check - may be empty. Note, if not, ordered such that the model with most is highlighted.
    // query_check_first( query );
    ui->input_model_comboBox->clear();
    while ( query.next() ) {
        ui->input_model_comboBox->addItem( query.value(0).toString() );
    }
    //ui->input_model_comboBox->addItem( "" );
}

void AddNewItemDialog::add_item(const QString &item_id) {
    Logger log( "void AddNewItemDialog::add_item(const QString &item_id)" );
    log.stream() << "item_id = '" << item_id << "'";
    doAddItem( item_id );
}

void AddNewItemDialog::doAddItem(QString id) {
    Logger log( "void AddNewItemDialog::doAddItem(QString id)" );
    PROTECT_BLOCK(
    log.stream() << "Item id is " << id;
    log.stream() << "Item id in %" << id.toLocal8Bit().toPercentEncoding();
    bool ok;
    long price = ui->input_price_lineEdit->text().toLong(&ok);
    if ( ! ok ) {
        log.stream( error ) << "Error converting price '" << ui->input_price_lineEdit->text() << "' to long. Using value of 0";
        price = 0;
    }
    // Use id from call here - as it can be provided by barcode scanner...
    Item new_item( id, ui->input_type_comboBox->currentText(), ui->input_size_lineEdit->text(),
                   ui->input_mark_comboBox->currentText(), ui->input_model_comboBox->currentText(),
                   ui->input_year_lineEdit->text(), ui->input_condition_comboBox->currentText(),
                   price, ui->input_rentalgroup_comboBox->currentText(), ui->input_note_textEdit->toPlainText() );
    log.stream() << "Starting transaction";
    /*if ( ! QSqlDatabase::database().transaction() ) {
        throw Exception( Errors::DBError )
            << ( log.stream( error ) << "Unable to start transaction" );
    }*/
    new_item.db_insert();
    /*if ( ! QSqlDatabase::database().commit() ) {
        throw Exception( Errors::DBError )
            << ( log.stream( error ) << "Unable to commit transaction" );
    }*/
    items_added.push( new_item );
    updateLogWindow();
    ui->input_id_lineEdit->setText("");
    ui->input_size_lineEdit->setFocus();
    );
}

void AddNewItemDialog::updateLogWindow() {
    Logger log( "void AddNewItemDialog::updateLogWindow()" );
    Item item;
    QString tmp;
    foreach ( item, items_added ) {
        tmp = tr( "<b>Added item: </b>" ) + item.toHtml() + tmp;
    }
    ui->output_log_textEdit->setText( tmp );
    // This is a little malplaced in here, but...
    ui->input_undo_pushButton->setEnabled( !items_added.isEmpty() );
}

void AddNewItemDialog::doUndo() {
    Logger log( "void AddNewItemDialog::doUndo()" );
    if ( items_added.isEmpty() ) {
        log.stream( error ) << "Undo with no items added - ignoring ";
        ui->input_undo_pushButton->setEnabled( false );
        return;
    }
    Item item( items_added.top() );
    // Check that id is not ''
    if ( item.m_id.isEmpty() ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Item that must be removed following undo, has empty id" );
    }
    QSqlQuery query;

    query_check_prepare( query, "delete from items where id = :id" );
    query.bindValue( ":id", item.m_id );
    query_check_exec( query );
    log.stream() <<  "Removed item with id " << item.m_id << " from database.";
    // Remove from internal stack, update.
    items_added.pop();
    // Note in the item log
    log.stream() << "Adding log line to itemevents";
    query_check_prepare( query, "insert into itemevents ( item_id, time, event, note ) "
                         "values ( :item_id, :time, :event, :note )" );
    query.bindValue( ":item_id", item.m_id );
    query.bindValue( ":time", QDateTime::currentDateTime() );
    query.bindValue( ":event", DB::Item::Event::deleted );
    query.bindValue( ":note", tr( "Deleted by undo in Add New Item dialog." ) );
    query_check_exec( query );

    updateLogWindow();
}

/////////////////////////////////////

void AddNewItemDialog::on_input_type_comboBox_currentIndexChanged(QString )
{
    Logger log( "void AddNewItemDialog::on_input_type_comboBox_currentIndexChanged(QString )" );
    doSetupBasedOnType();
}

void AddNewItemDialog::on_input_mark_comboBox_currentIndexChanged(QString )
{
    Logger log("void AddNewItemDialog::on_input_mark_comboBox_currentIndexChanged(QString )");
    doSetupBasedOnMark();
}

void AddNewItemDialog::on_input_add_pushButton_clicked()
{
    Logger log( "void AddNewItemDialog::on_input_add_pushButton_clicked()" );
    doAddItem( ui->input_id_lineEdit->text());
}

void AddNewItemDialog::on_input_id_lineEdit_textChanged(QString value)
{
    // Only allow clicking on add, if id is not empty.
    ui->input_add_pushButton->setEnabled( ! value.isEmpty() );
}

void AddNewItemDialog::on_input_done_pushButton_clicked()
{
    close();
}

void AddNewItemDialog::on_input_undo_pushButton_clicked()
{
    Logger log( "void AddNewItemDialog::on_input_undo_pushButton_clicked()" );
    doUndo();
}
