#include "stockstatusdialog.h"
#include "ui_stockstatusdialog.h"

// Qt
#include <QSqlQuery>
#include <QVariant>
#include <QMessageBox>

// App
#include "utility.h"
#include "log.h"
#include "db_consts.h"
#include "printerhelpers.h"
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
    QSqlQuery query = createItemQuery( "id, type, size, mark, model, year, condition, price, rentalgroup, state, note",
                                       ui->input_from_dateEdit->date(),
                                       ui->input_to_dateEdit->date(),
                                       "order by id" );
    /*
    query_check_prepare( query, "select * from items "
                                "where state not in ( :state_lost, :state_discarded ) "
                                "and not exists "
                                "  ( select id from itemevents "
                                "    where item_id = items.id "
                                "    and event = :state_status "
                                "    and time >= :fromdate "
                                "    and time <= :todate ) "
                                "order by id");
    query.bindValue( ":state_lost",      DB::Item::Event::lost );
    query.bindValue( ":state_discarded", DB::Item::Event::discarded );
    query.bindValue( ":state_status",    DB::Item::Event::status );
    query.bindValue( ":fromdate", ui->input_from_dateEdit->date() );
    query.bindValue( ":todate", ui->input_to_dateEdit->date() ); */
    query_check_exec( query );
    item_model.setQuery( query );
    // TODO: Fix header translations / select headers.
    ui->output_items_tableView->setModel( &item_model );
    ui->output_items_tableView->resizeColumnsToContents();
}

QSqlQuery StockStatusDialog::createItemQuery( const QString & columnClause,
                                              const QDate & fromdate,
                                              const QDate & todate,
                                              const QString & orderClause ) {
    Logger log( "QSqlQuery StockStatusDialog::createItemQuery( bool detailed, const QDate & fromdate, const QDate & todate )" );
    QSqlQuery query;
    query_check_prepare( query, "select " + columnClause + " from items "
                                "where state not in ( :state_lost, :state_discarded ) "
                                "and not exists "
                                "  ( select id from itemevents "
                                "    where item_id = items.id "
                                "    and event = :state_status "
                                "    and time >= :fromdate "
                                "    and time <= :todate ) "
                                + orderClause );
    query.bindValue( ":state_lost",      DB::Item::Event::lost );
    query.bindValue( ":state_discarded", DB::Item::Event::discarded );
    query.bindValue( ":state_status",    DB::Item::Event::status );
    query.bindValue( ":fromdate", fromdate );
    query.bindValue( ":todate", todate );
    return query;
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
    ui->output_itemevents_tableView->setModel( NULL );
}

void StockStatusDialog::on_input_to_dateEdit_editingFinished()
{
    Logger log("void StockStatusDialog::on_input_to_dateEdit_editingFinished()");
    updateItemModel();
}

void StockStatusDialog::on_output_items_tableView_activated(const QModelIndex &index)
{
    Logger log("void StockStatusDialog::on_output_items_tableView_activated(const QModelIndex &index)");
    // Get id from coloum 0
    QModelIndex myIndex = item_model.index( index.row(), 0 );
    QString item_id = item_model.data( myIndex ).toString();
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

void StockStatusDialog::on_input_print_pushButton_clicked()
{
    Logger log("void StockStatusDialog::on_input_print_pushButton_clicked()");
    // Print the item ids of all the items from the itemmodel.
    QSqlQuery query = createItemQuery( "id, type, size", ui->input_from_dateEdit->date(), ui->input_to_dateEdit->date(),
                                       "order by type, size, id" );
    if ( PrinterHelpers::doItemSplitPrint( tr( "List of missing items" ),
                                      tr( "Printed at ") + QDateTime::currentDateTime().date().toString(),
                                      query, true, 1, 10 ) ) {
        QMessageBox::information( this, tr( "List printed" ),
                                  tr( "List of missing items printed" ) );
    }

}

void StockStatusDialog::on_input_savereport_pushButton_clicked()
{
    Logger log("void StockStatusDialog::on_input_savereport_pushButton_clicked()");
    TODO("Implement print functionality");
}
