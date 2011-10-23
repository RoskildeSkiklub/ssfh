#include "reiddialog.h"
#include "ui_reiddialog.h"

// Qt
#include <QAbstractButton>
#include <QMessageBox>
#include <QRegExp>
#include <QValidator>

// app
#include "log.h"
#include "globals.h"
#include "exception.h"
#include "utility.h"
#include "item.h"

using namespace Log;

ReIdDialog::ReIdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReIdDialog)
{
    ui->setupUi(this);

    // Set validators for to_id lineedit.
    // Technically, one could envision ids different from only numbers...
    QRegExp rx( "\\d+" );
    QValidator *validator = new QRegExpValidator( rx, this);
    ui->input_toId_lineEdit->setValidator( validator );

    // Clear the fields, set enabled on buttons, etc.
    clear();

    // Get the signals from the scan input events.
    connect( Globals::interceptor, SIGNAL(barcodeItemScan(QString)),
             this, SLOT(scan_item(QString) ) );
    //! \todo Provide audio feedback when returning/adding items
    /* \todo: connect(this, SIGNAL(item_returned()),
            Globals::getFeedbackObject(), SLOT(itemReturned()));
    connect(this, SIGNAL(item_added()),
            Globals::getFeedbackObject(), SLOT(itemAdded())); */
}

void ReIdDialog::clear() {
    Logger log( "void ReIdDialog::clear()" );
    //! \todo Fix up

    // Clear and enable fromId lineEdit
    ui->input_fromId_lineEdit->setText( "" );
    ui->input_fromId_lineEdit->setReadOnly( false );

    // Lookup is not enabled, fromId is blank
    ui->input_lookup_pushButton->setEnabled( false );

    // ToId not enabled, until lookup has been performed
    ui->input_toId_lineEdit->setText( "");
    ui->input_toId_lineEdit->setEnabled( false );

    // Clear the item description
    ui->output_contract_textBrowser->setText( "" );

    // Disable accept/ok button
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    ui->buttonBox->button( QDialogButtonBox::Apply )->setEnabled( false );

    // Set focus to fromId
    ui->input_fromId_lineEdit->setFocus();
}

ReIdDialog::~ReIdDialog()
{
    delete ui;
}

void ReIdDialog::scan_item(const QString &item_id) {
    Logger log("ReidDialog::scan_item(const QString &item_id)");
    // Use readonly status of fromId to determine where we are...

    if ( ui->input_fromId_lineEdit->isReadOnly() ) {
        log.stream() << "fromId is readonly, inputting stuff into toId";
        // Insert into rentItem and do the swap.
        ui->input_toId_lineEdit->setText( item_id );
        //! \todo
        // rentItemSet();
        // emit item_added();
    } else {
        log.stream() << "fromId is writeable, inputting stuff into fromId";
        // Into returnItem, and try to return
        ui->input_fromId_lineEdit->setText( item_id );
        //! \todo
        fromIdSet();
        // emit item_returned();
    }
}

void ReIdDialog::fromIdSet() {
    Logger log("ReidDialog::fromIdSet()");
    if ( ui->input_fromId_lineEdit->text().isEmpty() ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error )
                     << "ReidDialog::fromIdSet called with empty fromId" );
    }
    // Locate item
    QString fromId = ui->input_fromId_lineEdit->text();
    Item item = Item::db_load( fromId );
    //! \todo - nice error, if item does not exist..
    log.stream() << "Item with id '" << item.getId() << "' loaded from database";
    ui->output_contract_textBrowser->setText( item.toHtml() );
    // Lock the field
    ui->input_fromId_lineEdit->setReadOnly( true );
    ui->input_lookup_pushButton->setEnabled( false );
    ui->input_toId_lineEdit->setEnabled( true );
    ui->input_toId_lineEdit->setFocus();
    //! \todo
    // ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ui->input_returnItem_lineEdit->isReadOnly() && !ui->input_rentItem_lineEdit->text().isEmpty() );
}

bool ReIdDialog::doReId() {
    Logger log( "void ReidDialog::doReId()" );
    QString from_id = ui->input_fromId_lineEdit->text();
    QString to_id = ui->input_toId_lineEdit->text();
    try {
        Item::db_reid( from_id, to_id );
    }
    catch ( const Exception & ex ) {
        log.stream( debug ) << "Item::db_reid failed, checking reason";
        switch ( ex.getStatusCode() ) {
        case Errors::ItemDoesNotExist: {
            QMessageBox::critical( this, tr( "Item does not exist"),
                                  tr( "The item with id '%1' was not found in the database, so it can not be given the new id '%2'.").arg( from_id ).arg( to_id ) );
            return false;
        }
        case Errors::ItemAlreadyExists: {
            QMessageBox::critical( this, tr( "Item already exist"),
                                  tr( "The item with id '%1' was found in the database, so the item with id '%2' can not be given the new id '%1'.").arg( to_id ).arg( from_id ) );
            return false;
        }
        default: {
            log.stream( error ) << "Unexpected exception from Item::db_reid : " << ex.toString();
            throw;
        }
        }
    }
    QMessageBox::information( this, tr( "Item has been given a new id"),
                             tr( "The item with id '%1' now have id '%2'").arg( from_id).arg( to_id ) );
    clear();
    return true;
}

void ReIdDialog::on_input_lookup_pushButton_clicked()
{
    Logger log( "void ReidDialog::on_input_lookup_pushButton_clicked()" );
    fromIdSet();
}

void ReIdDialog::on_input_fromId_lineEdit_textChanged( QString value ) {
    Logger log("void ReIdDialog::on_input_fromId_lineEdit_textChanged( QString )");
    ui->input_lookup_pushButton->setEnabled( !value.isEmpty() );
}


void ReIdDialog::on_input_toId_lineEdit_textChanged(QString )
{
    Logger log("void ReIdDialog::on_input_toId_lineEdit_textChanged(QString )");
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ui->input_fromId_lineEdit->isReadOnly() && !ui->input_toId_lineEdit->text().isEmpty() );
    ui->buttonBox->button( QDialogButtonBox::Apply )->setEnabled( ui->input_fromId_lineEdit->isReadOnly() && !ui->input_toId_lineEdit->text().isEmpty() );
}

// OK, as Apply, but also closes window
void ReIdDialog::on_buttonBox_accepted()
{
    Logger log( "void ReIdDialog::on_buttonBox_accepted()" );
    if ( doReId() )  {
        close();
    }
}

void ReIdDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    Logger log( "void ReIdDialog::on_buttonBox_clicked(QAbstractButton* button)" );
    if ( button == ui->buttonBox->button( QDialogButtonBox::Apply) ) {
        log.stream() << "Apply pressed";
        doReId();
    } else {
        log.stream(  warn ) << "Unknown button pressed";
    }
}

void ReIdDialog::on_pushButton_clicked()
{
    Logger log("void ReIdDialog::on_pushButton_clicked()");
    TODO( "Implement file reading and stuff" );
}
