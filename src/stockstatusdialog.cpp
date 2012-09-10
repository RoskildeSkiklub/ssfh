#include "stockstatusdialog.h"
#include "ui_stockstatusdialog.h"

// Qt
#include <QSqlQuery>
#include <QVariant>

// App
#include "utility.h"
#include "log.h"
#include "db_consts.h"
using namespace Log;

StockStatusDialog::StockStatusDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StockStatusDialog)
{
    ui->setupUi(this);

    // Set the dates of the input stuff
    ui->input_from_dateEdit->setDate( QDate::currentDate() );
    ui->input_to_dateEdit->setDate( QDate::currentDate() );

    // Setup the query model for the items
    updateItemModel();
}


StockStatusDialog::~StockStatusDialog()
{
    delete ui;
}

void StockStatusDialog::updateItemModel() {
    Logger log("StockStatusDialog::updateItemModel()");

    // Setup the query model for the items
    QSqlQuery query;
    query_check_prepare( query, "select * from items "
                                "where state not in ( :state_lost, :state_discarded ) "
                                "and not exists "
                                "  ( select id from itemevents "
                                "    where item_id = items.id "
                                "    and event = :state_status "
                                "    and time >= :fromdate "
                                "    and time <= :todate )" );
    query.bindValue( ":state_lost",      DB::Item::Event::lost );
    query.bindValue( ":state_discarded", DB::Item::Event::discarded );
    query.bindValue( ":state_status",    DB::Item::Event::status );
    query.bindValue( ":fromdate", ui->input_from_dateEdit->date() );
    query.bindValue( ":todate", ui->input_to_dateEdit->date() );
    query_check_exec( query );
    item_model.setQuery( query );
    // TODO: Fix header translations / select headers.
    ui->output_items_tableView->setModel( &item_model );
    ui->output_items_tableView->resizeColumnsToContents();


}


void StockStatusDialog::on_input_print_pushButton_clicked()
{
    Logger log("void StockStatusDialog::on_input_print_pushButton_clicked()");
    TODO("Implement");
}

void StockStatusDialog::on_input_close_pushButton_clicked()
{
    Logger log("void StockStatusDialog::on_input_close_pushButton_clicked()");
    close();
}

void StockStatusDialog::on_input_from_dateEdit_editingFinished()
{
    Logger log("void StockStatusDialog::on_input_from_dateEdit_editingFinished()");
    updateItemModel();
}

void StockStatusDialog::on_input_to_dateEdit_editingFinished()
{
    Logger log("void StockStatusDialog::on_input_to_dateEdit_editingFinished()");
    updateItemModel();
}
