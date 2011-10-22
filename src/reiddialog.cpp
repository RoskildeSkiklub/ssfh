#include "reiddialog.h"
#include "ui_reiddialog.h"

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

    //! \todo Fix up
    // Disable accept/ok button
    // ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    // Get the signals from the scan input events.
    connect( Globals::interceptor, SIGNAL(barcodeItemScan(QString)),
             this, SLOT(scan_item(QString) ) );
    // Provide audio feedback when returning/adding items
    /* \todo: connect(this, SIGNAL(item_returned()),
            Globals::getFeedbackObject(), SLOT(itemReturned()));
    connect(this, SIGNAL(item_added()),
            Globals::getFeedbackObject(), SLOT(itemAdded())); */


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
    //! \todo
    // ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ui->input_returnItem_lineEdit->isReadOnly() && !ui->input_rentItem_lineEdit->text().isEmpty() );
}

void ReIdDialog::toIdSet() {
    Logger log( "void ReidDialog::toIdSet()" );
    TODO( "Implement ReidDialog::toIdSet" );
}

void ReIdDialog::on_input_lookup_pushButton_clicked()
{
    Logger log( "void ReidDialog::on_input_lookup_pushButton_clicked()" );
    toIdSet();
}
