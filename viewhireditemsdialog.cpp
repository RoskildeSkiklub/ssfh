#include "viewhireditemsdialog.h"
#include "ui_viewhireditemsdialog.h"

// Qt
#include <QSqlQuery>

// app
#include "log.h"
#include "utility.h"
#include "db_consts.h"
#include "contract.h"

using namespace Log;

ViewHiredItemsDialog::ViewHiredItemsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewHiredItemsDialog)
{
    ui->setupUi(this);

    // Fix up the query model to select all items that are out
    model = new QSqlQueryModel;
    QSqlQuery query;
    query_check_prepare( query, "select id, type, size, mark, model, year, condition, price, rentalgroup, note "
                         "from items "
                         "where state = :state");
    query.bindValue( ":state", DB::Item::State::out );
    query_check_exec( query );
    model->setQuery( query );
    model->setHeaderData( 0, Qt::Horizontal, tr( "Id" ) );
    model->setHeaderData( 1, Qt::Horizontal, tr( "Type" ) );
    model->setHeaderData( 2, Qt::Horizontal, tr( "Size" ) );
    model->setHeaderData( 3, Qt::Horizontal, tr( "Mark" ) );
    model->setHeaderData( 4, Qt::Horizontal, tr( "Model" ) );
    model->setHeaderData( 5, Qt::Horizontal, tr( "Year" ) );
    model->setHeaderData( 6, Qt::Horizontal, tr( "Condition" ) );
    model->setHeaderData( 7, Qt::Horizontal, tr( "Price" ) );
    model->setHeaderData( 8, Qt::Horizontal, tr( "Rentalgroup" ) );
    model->setHeaderData( 9, Qt::Horizontal, tr( "Note" ) );

    ui->item_tableView->setModel( model );


}

ViewHiredItemsDialog::~ViewHiredItemsDialog()
{
    delete ui;
}

void ViewHiredItemsDialog::on_item_tableView_activated(QModelIndex index) {
    Logger log("void ViewHiredItemsDialog::on_item_tableView_activated(QModelIndex index)");
    // Get id from column 0
    QModelIndex myIndex = model->index( index.row(), 0 );
    qlonglong item_id = model->data( myIndex ).toLongLong();
    // Get the first active contract for this index
    log.stream() << "Got item_id '" << item_id
            << "', trying to locate contract.";
    QSqlQuery query;
    query_check_prepare( query, "select contract_id "
                         "from contractItems "
                         "where item_id = :item_id "
                         "and state = :state " );
    query.bindValue( ":item_id", item_id );
    query.bindValue( ":state", DB::ContractItem::State::out );
    query_check_exec( query );
    if ( query.first() ) {
        qlonglong contract_id = query.value(0).toLongLong();
        log.stream() << "Found contract with id '"
                << contract_id << "'";
        // Load it, set the text
        ui->ouput_contract_textBrowser
                ->setHtml( Contract::db_load( contract_id ).toReturnHtml() );
    } else {
        ui->ouput_contract_textBrowser->setHtml( tr( "No contract found" ) );
    }
}
