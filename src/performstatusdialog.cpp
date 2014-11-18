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

#include "performstatusdialog.h"
#include "ui_performstatusdialog.h"

// Qt
#include "QMessageBox"
#include "QFileDialog"

// app
#include "log.h"
#include "globals.h"
// #include "utility.h"
#include "db_consts.h"
#include "item.h"
#include "exception.h"
#include "printerhelpers.h"

using namespace Log;

PerformStatusDialog::PerformStatusDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PerformStatusDialog)
{
    ui->setupUi(this);

    // Get the signals from the scan input events.
    connect( Globals::interceptor, SIGNAL(barcodeItemScan(QString)),
             this, SLOT(scan_item(QString) ) );
    //! \todo Provide audio feedback when scanning items
    connect(this, SIGNAL(item_scanned()),
            Globals::getFeedbackObject(), SLOT(itemScanned()));
}

PerformStatusDialog::~PerformStatusDialog()
{
    delete ui;
}

// Signals and slots.


void PerformStatusDialog::scan_item(const QString &item_id) {
    Logger log("PerformStatusDialog::scan_item(const QString &item_id)");
    log.stream() << "item_id = '" << item_id << "' was scanned for status.";
    try {
        Item item = Item::db_load( item_id );
        // Set the status and give user feedback.
        item.addEventLine( DB::Item::Event::status, "Present" );
        emit item_scanned(); // Hits feedback system
        ui->output_lastscanneditem_textBrowser->setText(item_id);
        ui->output_recentlyscanneditems_textBrowser->setText( item_id + "\n" + ui->output_recentlyscanneditems_textBrowser->toPlainText() );

        // Update description, and track problem items.
        ui->output_itemdescription_textBrowser->setText( item.toHtml() );
        if ( item.getState() != DB::Item::State::in ) {
            // No message box for this, user can print
            ui->output_scannednotinitems_textBrowser->setText( item_id + "\n" + ui->output_scannednotinitems_textBrowser->toPlainText() );
            log.stream( warn ) << "The following item was present, but was not marked as 'in', item_id = '" << item_id << "'. Details: " << item.toString();
            ui->input_print_pushButton->setEnabled( true );
            ui->input_save_pushButton->setEnabled( true );
        }
    } catch ( const Exception & e ) {
        if ( e.getStatusCode() == Errors::ItemDoesNotExist ) {
            // TODO: Emit unknown sound, when feedback system fixed...
            QMessageBox::warning( this, tr( "Item not registered in database" ),
                                  tr( "Item with id '%0' was not found in database. Unable to register for status.").arg( item_id ) );
            return;
        } else {
            throw;
        }
    }
}



void PerformStatusDialog::on_input_close_pushButton_clicked() {
    Logger log("void PerformStatusDialog::on_input_close_pushButton_clicked()");
    close();
}

void PerformStatusDialog::on_input_save_pushButton_clicked()
{
    Logger log("void PerformStatusDialog::on_input_save_pushButton_clicked()");
    QString filename = QFileDialog::getSaveFileName( this, tr("Save list of missing items"),
                                                     "",
                                                     tr("Text files (*.txt)") );
    if ( filename.isEmpty() ) {
        log.stream() << "No file selected, aborting";
        return;
    }
    QFile file( filename );
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    if ( !file.isOpen() ) {
        QMessageBox::warning(this, tr( "Failed to open file for writing"),
                             tr( "Could not write to selected file '%1'").arg( filename ) );
        return;
    }
    QTextStream out(&file);
    out << ui->output_scannednotinitems_textBrowser->toPlainText();
    file.close();
    QMessageBox::information( this, tr( "File written" ), tr( "Information about items that were present, but not marked as in, was written to file '%1'." ).arg( filename ) );
}

void PerformStatusDialog::on_input_print_pushButton_clicked()
{
    Logger log( "void PerformStatusDialog::on_input_print_pushButton_clicked()" );
    if ( PrinterHelpers::printStringsAsBarcodes( tr( "List of items present, but not in" ),
                ui->output_scannednotinitems_textBrowser->toPlainText().split( QRegExp( "\n") ),
                                                 "" ) ) {
        QMessageBox::information( this, tr( "Item Ids printed"),
                                 tr( "The item ids have been printed as barcodes") );
    }
}
