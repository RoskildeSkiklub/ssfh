#include "trackitemdialog.h"
#include "ui_trackitemdialog.h"

// Qt
#include <QMessageBox>
#include <QSqlQuery>

// app
#include "log.h"
#include "globals.h"
// #include "exception.h"
#include "utility.h"
#include "item.h"
#include "db_consts.h"

using namespace Log;

TrackItemDialog::TrackItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrackItemDialog)
{
    ui->setupUi(this);

/*
    // Set validators for to_id lineedit.
    // Technically, one could envision ids different from only numbers...
    QRegExp rx( "\\d+" );
    QValidator *validator = new QRegExpValidator( rx, this);
    ui->input_toId_lineEdit->setValidator( validator );
*/

    // Clear the fields, set enabled on buttons, etc.
    clear();

    // Get the signals from the scan input events.
    connect( Globals::interceptor, SIGNAL(barcodeItemScan(QString)),
             this, SLOT(scan_item(QString) ) );
    //! \todo Provide audio feedback when returning/adding items
    connect(this, SIGNAL(item_scanned()),
            Globals::getFeedbackObject(), SLOT(itemScanned()));
}

TrackItemDialog::~TrackItemDialog()
{
    delete ui;
}

void TrackItemDialog::scan_item(const QString &item_id) {
    Logger log("void TrackItemDialog::scan_item(const QString &item_id)");
    ui->input_id_lineEdit->setText( item_id );
    emit item_scanned();
    doTrackItem();
}

void TrackItemDialog::doTrackItem() {
    Logger log("void TrackItemDialog::doTrackItem()");
    // Try and see if the item_id in input_id_lineEdit exists
    clear();
    QString item_id = ui->input_id_lineEdit->text();
    log.stream() << "Looking for item with id = " << item_id;
    if ( ! Item::db_exists( item_id ) ) {
        log.stream() << "Item with id = " << item_id << " not found, asking user about re-id";
        if ( QMessageBox::Yes ==
                QMessageBox::question(this, tr( "Item not found" ) ,
                                      tr( "The item with id '%1' was not found. Would you like to look for a re-id'et item?").arg( item_id ),
                                      QMessageBox::Yes | QMessageBox::No ) ) {
            // TODO: Move this to a method on items? itemevents? Something?
            QSqlQuery query;
            query_check_prepare( query,
                                 "select item_id from itemevents "
                                 "where event = :event "
                                 "and note like :like" );
            query.bindValue( ":event", DB::Item::Event::reid );
            query.bindValue( ":like", "%from id '" + item_id + "'%" );
            query_check_exec( query );
            if ( ! query.first() ) {
                log.stream() << "No re-id records found for item with id = " << item_id;
                QMessageBox::information( this, tr( "Item not found" ),
                                          tr( "No re-id records found for item with id '%1'.").arg(item_id) );
                return;
            }
            QString new_item_id = query.value(0).toString();
            QMessageBox::information( this, tr( "Item tracked" ),
                                      tr( "Found that item '%1' has been re-id'et to item '%2'.")
                                      .arg( item_id ).arg( new_item_id ) );
            item_id = new_item_id;
        } else {
            return;
        }
    }
    log.stream() << "Using item with id = " << item_id << " for tracking.";

    TODO( "Implement rest of doTrackItem " );

}

void TrackItemDialog::clear() {
    Logger log("void TrackItemDialog::clear()");
    // TODO: Clear the model for the events.

}

void TrackItemDialog::on_input_close_pushButton_clicked()
{
    Logger log("void TrackItemDialog::on_input_close_pushButton_clicked()");
    close();
}

void TrackItemDialog::on_input_lookup_pushButton_clicked()
{
    Logger log("void TrackItemDialog::on_input_lookup_pushButton_clicked()");
    doTrackItem();
}
