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
    QString item_id = ui->input_id_lineEdit->text();
    log.stream() << "Looking for item with id = " << item_id;
    if ( ! Item::db_exists( item_id ) ) {
        log.stream() << "Item with id = " << item_id << " not found, checking for a re-id";
        // TODO: Move this lookup to a method on items? itemevents? Something?
        QSqlQuery query;
        query_check_prepare( query,
                             "select item_id from itemevents "
                             "where event = :event "
                             "and note like :like" );
        query.bindValue( ":event", DB::Item::Event::reid );
        query.bindValue( ":like", "%from id '" + item_id + "'%" );
        query_check_exec( query );

        if ( query.first() ) {
            QString reid_item_id = query.value(0).toString();
            log.stream() << "Found re-id id " << reid_item_id
                         << " for item with original id " << item_id;
            QMessageBox::information( this, tr( "Item found" ),
                                      tr( "Found that item '%1' has been re-id'et to item '%2', using that instead.")
                                      .arg( item_id ).arg( reid_item_id ) );
            item_id = reid_item_id;
        } else {
            log.stream() << "No re-id records found for item with id = " << item_id;
            QMessageBox::information( this, tr( "Item not found" ),
                                      tr( "The item with id '%1' was not found, and no re-id records could be found for it" ).arg(item_id) );
            return;
        }
    }
    // Item found, possibly with other id, proceed.
    log.stream() << "Using item with id = " << item_id << " for tracking.";
    clear();
    ui->input_id_lineEdit->setText( item_id );
    Item item = Item::db_load( item_id );
    ui->output_item_textBrowser->setText( item.toString() );
    updateItemeventsModel();
}

void TrackItemDialog::clear() {
    Logger log("void TrackItemDialog::clear()");
    // TODO: Clear the model for the events.

}

void TrackItemDialog::updateItemeventsModel() {
    Logger log("void TrackItemDialog::updateItemeventsModel()");
    QString item_id = ui->input_id_lineEdit->text();
    log.stream() << "Selected item with id '" << item_id << "'";

    // Load it, set the text
    // TODO: Move this somewhere else.
    // Setup the query model for the itemevents
    QSqlQuery query;
    query_check_prepare( query, "select * from itemevents "
                                "where item_id = :item_id "
                                "order by time desc" );
    query.bindValue( ":item_id", item_id );
    query_check_exec( query );
    itemevents_model.setQuery( query );
    // TODO: Fix header translations / select headers.
    ui->output_itemevents_tableView->setModel( &itemevents_model );
    ui->output_itemevents_tableView->resizeColumnsToContents();
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
    ui->input_id_lineEdit->setFocus();
}
