#include "printchecklistdialog.h"
#include "ui_printchecklistdialog.h"

// Qt
#include <QSqlQuery>
#include <QMessageBox>
#include <QVariant>
#include <QString>

// app
#include "log.h"
#include "globals.h"
#include "utility.h"
#include "db_consts.h"
#include "item.h"

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
    // Get the level of sorting from the dialog
    int sortLevel = ui->input_splitSortLevels_spinBox->value();
    int maxItems = ui->input_maxItemsPerPage_spinBox->value();

    // Build the query from the sortlevels.
    QString sortString = sortList.join( ", " );
    QString prepostfix;
    if ( !sortString.isEmpty() ) {
        prepostfix = ", ";
    }
    QString queryString = QString( "select id %0%1 from items where state = :state order by %1%0 id" )
                          .arg( prepostfix ).arg( sortString );

    // Now, actually do it.
    QSqlQuery query;
    query_check_prepare( query, queryString );
    query.bindValue( ":state", ui->input_itemState_comboBox->itemData(
            ui->input_itemState_comboBox->currentIndex() ) );
    query_check_exec( query );
    if ( query.first() ) {
        if ( Globals::checkPosPrinter() ) {
            int count = 0;


            Pos::Printer & posp( Globals::getPosPrinter() );
            posp.setFontSize( 1, 2 );

            bool needToStartReceipt = true;
            bool shouldEndReceipt = false;

            log.stream( info ) << "Starting print of check list";
            do {
                // Start new receipt, if needed, set up for final closure.
                if ( needToStartReceipt ) {
                    posp.startReceipt();
                    needToStartReceipt = false;
                    posp << tr( "List of items with state: " )
                            << ui->input_itemState_comboBox->currentText();
                    posp << Pos::hr;
                    shouldEndReceipt = true;
                }

                // Print items.
                // posp << Pos::endl;
                QString item_id = query.value(0).toString();
                Item item( Item::db_load( item_id ) );
                posp << item.toReceiptString() << Pos::endl;
                // posp << Pos::center << Pos::Barcode( item_id ) << Pos::hr;

                // Check if we should do a pagebreak now. Base it on count for now.
                ++count;
                if ( ( count % maxItems ) == 0 ) {
                    posp.endReceipt();
                    needToStartReceipt = true;
                    shouldEndReceipt = false;
                }
            } while( query.next() );

            if ( shouldEndReceipt ) {
                posp.endReceipt();
            }
            posp.setFontSize();
            QMessageBox::information( this, tr("Checklist printed"),
                                      tr( "Checklist printed" ) );
        }
    } else {
        QMessageBox::information( this, tr("No items found"),
                                  tr( "No missing items found") );
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
        res = tr( "<Nothing>" );
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
    doPrint();
}

void PrintChecklistDialog::on_input_itemState_comboBox_currentIndexChanged(int index) {
    Logger log("void PrintChecklistDialog::on_input_itemState_comboBox_currentIndexChanged(int index)");
    updateItemsCount();
}



void PrintChecklistDialog::on_input_sortBy_comboBox_currentIndexChanged(int index) {
    Logger log("void PrintChecklistDialog::on_input_sortBy_comboBox_currentIndexChanged(int index)");
    if ( ui->input_itemState_comboBox->currentIndex() >= 0 ) {
        ui->input_addSortCriteria_pushButton->setEnabled(
                ! sortList.contains(
                        ui->input_sortBy_comboBox->itemData(
                                ui->input_sortBy_comboBox->currentIndex() ).toString() ) );
    }
}
