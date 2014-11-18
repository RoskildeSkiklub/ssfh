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

#include "printchecklistdialog.h"
#include "ui_printchecklistdialog.h"

// Qt
#include <QSqlQuery>
#include <QMessageBox>
#include <QVariant>
#include <QString>
#include <QVector>
#include <QtGlobal>
#include <QDateTime>

// app
#include "log.h"
#include "globals.h"
#include "utility.h"
#include "db_consts.h"
#include "item.h"
#include "printerhelpers.h"

using namespace Log;

PrintChecklistDialog::PrintChecklistDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintChecklistDialog)
{
    Logger log("PrintChecklistDialog::PrintChecklistDialog(QWidget *parent)");

    ui->setupUi(this);

    // Fill in values from the database

    // Set up the items combo boxt
    QSqlQuery query;
    query_check_prepare( query, "select distinct state from items");
    query_check_exec( query );
    query_check_first( query );
    do {
        ui->input_itemState_comboBox->addItem( Item::tr( query.value(0).toString().toLocal8Bit().constData() ),
                                              query.value(0) );
    } while( query.next() );
    // Set the combobox to point at the DB::Item::State::out value
    for( int i = 0; i < ui->input_itemState_comboBox->count(); ++i ) {
        if ( ui->input_itemState_comboBox->itemData(i).toString() == DB::Item::State::out ) {
            ui->input_itemState_comboBox->setCurrentIndex( i );
            break;
        }
    }
    updateItemsCount();

    // Set up the sorting items combo box
    ui->input_sortBy_comboBox->addItem( tr( "type" ), "type" );
    ui->input_sortBy_comboBox->addItem( tr( "size" ), "size" );
    ui->input_sortBy_comboBox->addItem( tr( "mark" ), "mark" );
    ui->input_sortBy_comboBox->addItem( tr( "model" ), "model" );
    ui->input_sortBy_comboBox->addItem( tr( "rentalgroup" ), "rentalgroup" );
    sortList << "type" << "size";
    updateSortDisplay();
    ui->input_sortBy_comboBox->setCurrentIndex( 2 );
}

PrintChecklistDialog::~PrintChecklistDialog()
{
    delete ui;
}

void PrintChecklistDialog::doPrint() {
    Logger log("void PrintChecklistDialog::doPrint()");

    if ( ui->input_itemState_comboBox->currentIndex() < 0 ) {
        return;
    }
    // Get the level of sorting from the dialog - limit to number of sortable items
    int sortLevel = qMin( ui->input_splitSortLevels_spinBox->value(), sortList.count( ) );
    int maxItems = ui->input_maxItemsPerPage_spinBox->value();

    // Build the query from the sortlevels.
    QString sortString = sortList.join( ", " );
    QString prepostfix;
    if ( !sortString.isEmpty() ) {
        prepostfix = ", ";
    }
    QString queryString = QString( "select id %0%1 from items where state = :state order by %1%0 id" )
                          .arg( prepostfix ).arg( sortString );

    // Now, actually print, prepare a query, call the splitPrint function.
    QSqlQuery query;
    query_check_prepare( query, queryString );
    query.bindValue( ":state", ui->input_itemState_comboBox->itemData(
            ui->input_itemState_comboBox->currentIndex() ) );

    if ( PrinterHelpers::printItemsSplitted( tr( "List of items with state: " ) + ui->input_itemState_comboBox->currentText(),
                                           tr( "Printed at " ) + QDateTime::currentDateTime().date().toString(),
                                           query, ui->input_includeBarcodes_checkBox->isChecked(),
                                           sortLevel, maxItems ) ) {
        QMessageBox::information( this, tr("Checklist printed"),
                                  tr( "Checklist printed" ) );
    }
}

void PrintChecklistDialog::updateItemsCount() {
    Logger log("void PrintChecklistDialog::updateItemsCount()");

    if ( ui->input_itemState_comboBox->currentIndex() >= 0 ) {
        QSqlQuery query;
        query_check_prepare( query, "select count(*) from items where state=:state");
        query.bindValue( ":state",
                         ui->input_itemState_comboBox->itemData( ui->input_itemState_comboBox->currentIndex() ) );
        query_check_exec( query );
        query_check_first( query );
        ui->output_numberOfMatchingItems_label->setText( QString( "%0").arg( query.value(0).toLongLong() ) );
    }
}

void PrintChecklistDialog::updateSortDisplay() {
    Logger log("void PrintChecklistDialog::updateSortDisplay()");
    QString res = "";
    if ( sortList.count() == 0 ) {
        res = tr( "<id>" );
    } else {
        QString prefix ="";
        for( int i = 0; i < sortList.count(); ++i) {
            res += prefix + tr( sortList.at(i).toLatin1().constData() );
            prefix = ", ";
        }
    }
    ui->output_sortBy_label->setText( res );
}

void PrintChecklistDialog::on_input_print_pushButton_clicked() {
    Logger log("void PrintChecklistDialog::on_input_print_pushButton_clicked()");
    bool ok;
    qlonglong numItems = ui->output_numberOfMatchingItems_label->text().toLongLong(&ok);
    if ( ok && numItems > 100 ) {
        if ( QMessageBox::Yes == QMessageBox::warning( this, tr("High number of items"),
                                                        tr( "The number of items in the checklist will be quite high.<br>"
                                                            "Are you sure you wish to print?" ),
                                                        QMessageBox::Yes | QMessageBox::No ) ) {
            doPrint();
        }
        return;
    }
    doPrint();
}

void PrintChecklistDialog::on_input_itemState_comboBox_currentIndexChanged(int) {
    Logger log("void PrintChecklistDialog::on_input_itemState_comboBox_currentIndexChanged(int index)");
    updateItemsCount();
}



void PrintChecklistDialog::on_input_sortBy_comboBox_currentIndexChanged(int) {
    Logger log("void PrintChecklistDialog::on_input_sortBy_comboBox_currentIndexChanged(int index)");
    if ( ui->input_itemState_comboBox->currentIndex() >= 0 ) {
        ui->input_addSortCriteria_pushButton->setEnabled(
                ! sortList.contains(
                        ui->input_sortBy_comboBox->itemData(
                                ui->input_sortBy_comboBox->currentIndex() ).toString() ) );
    }
}

void PrintChecklistDialog::on_input_addSortCriteria_pushButton_clicked() {
    Logger log("void PrintChecklistDialog::on_input_addSortCriteria_pushButton_clicked()");
    int ci = ui->input_sortBy_comboBox->currentIndex();
    if ( ci >= 0 ) {
        if ( !sortList.contains( ui->input_sortBy_comboBox->itemData( ci ).toString() ) ) {
            sortList.append( ui->input_sortBy_comboBox->itemData( ci ).toString() );
            updateSortDisplay();
        }
    }
    ui->input_addSortCriteria_pushButton->setEnabled( false );
    ui->input_removeSortCriteria_pushButton->setEnabled( true );
}

void PrintChecklistDialog::on_input_removeSortCriteria_pushButton_clicked() {
    Logger log("void PrintChecklistDialog::on_input_removeSortCriteria_pushButton_clicked()");
    sortList.removeLast();
    updateSortDisplay();
    ui->input_removeSortCriteria_pushButton->setEnabled( !sortList.isEmpty() );
    // More tricky - this could probably be handled better via some signals...
    int ci = ui->input_sortBy_comboBox->currentIndex();
    if ( ci >= 0 ) {
        if ( !sortList.contains( ui->input_sortBy_comboBox->itemData( ci ).toString() ) ) {
            ui->input_addSortCriteria_pushButton->setEnabled( true );
        }
    }
}

void PrintChecklistDialog::on_input_close_pushButton_clicked()
{
    Logger log("void PrintChecklistDialog::on_input_close_pushButton_clicked()");
    close();
}
