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

#include "stockstatusdialog.h"
#include "ui_stockstatusdialog.h"

// Qt
#include <QSqlQuery>
#include <QVariant>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>

// App
#include "utility.h"
#include "log.h"
#include "db_consts.h"
#include "printerhelpers.h"
#include "item.h"
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
    if ( PrinterHelpers::printItemsSplitted( tr( "List of missing items" ),
                                      tr( "Printed at ") + QDateTime::currentDateTime().date().toString(),
                                      query, true, 1, 10 ) ) {
        QMessageBox::information( this, tr( "List printed" ),
                                  tr( "List of missing items printed" ) );
    }

}

void StockStatusDialog::on_input_savereport_pushButton_clicked()
{
    Logger log("void StockStatusDialog::on_input_savereport_pushButton_clicked()");
    QString filename = QFileDialog::getSaveFileName( this, tr("Save HTML Report"),
                                                    "",
                                                    tr("Html files (*.html *.htm)") );
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
    // Do a report in HTML.
    // TODO: Clearly move this html report somewhere else/dynamic/something.
    /* Structure <html><head><title/></head><body>[items]
      [items] = item.toHtml() + itemevents table, limited to 10 */
    QStringList report;

    report << "<html><head>\n" << "<META HTTP-EQUIV='Content-Type' CONTENT='text/html; charset=UTF-8'>"
           << "<title>" << tr( "Report of missing items generated at " )
           << QDateTime::currentDateTime().date().toString() << "</title></head><body>";
    // Javascript part
    report << "<body onLoad='setupPage()' style='text-align: left; font-size: 12px;'>"
           << "<script type='text/javascript'>"
           << "var iteminfos = [ \n";

    QSqlQuery query = createItemQuery( "id, type, size",
                                       ui->input_from_dateEdit->date(), ui->input_to_dateEdit->date(),
                                       "order by type, size, id" );
    query_check_exec( query );

    if ( ! query.first() ) {
        // report << "<p>" << tr( "No items missing" ) << "</p>";
    } else {
        QSqlQuery query_events;
        QString item_id;
        do {
            item_id = query.value(0).toString();
            Item item( Item::db_load( item_id ) );
            report << "{id : '" + item_id + "', desc: '" + item.toHtml().replace( "\n", "" ) + "', \n events : [ ";
            query_check_prepare( query_events,
                                 "select time, event, note "
                                 "from itemevents "
                                 "where item_id = :item_id "
                                 "order by time desc" );
            query_events.bindValue( ":item_id", item_id );
            query_check_exec( query_events );
            if( ! query_events.first() ) {
                // report << "  <li>" << tr( "No events for item!" ) << "</li>\n";
            } else {
                do {
                    report << "'" << query_events.value( 0 ).toDateTime().toString( Qt::ISODate )
                           << " : " << Item::tr( query_events.value( 1 ).toString().toLocal8Bit().constData() )
                           << " : " << query_events.value( 2 ).toString().replace( "'", "\\'" ) << "',\n";
                } while( query_events.next() ); // itemsevents
            }
            report << " ]},\n"; // end itemevents and item
        } while( query.next() ); // itemsQuery

    }

    report << "]\n";

    report <<
    "var itemevents = {};\n"

    "// show/hide the itemevents\n"
    "var expandItemEvents = function( object, id ) {\n"
    "  expanded = itemevents[id].expanded;\n"
    "  var content = '';\n"
    "  var events = itemevents[id].events;\n"
    "  if ( !expanded ) {\n"
    "    for ( var i = 0; i < events.length; ++i ) {\n"
    "      content += events[i] + '<br>';\n"
    "      }\n"
    "  }\n"
    "  document.getElementById( 'itemdiv' + id ).innerHTML = content;\n"
    "  itemevents[id].expanded = !expanded; // toggle\n"
    "}\n"

    "var setupPage = function() {\n"
    "  var content = '';\n"
    "  for ( var i = 0; i < iteminfos.length ; ++i ) {\n"
    "    content += '<h2>' + iteminfos[i].id + '</h2>'\n"
    "             + iteminfos[i].desc\n"
    "            +  \"<button onclick='expandItemEvents( this, \\\"\" + iteminfos[i].id + \"\\\" )'/>Vis/skjul itemevents</button>\"\n"
    "            +  \"<div id='itemdiv\" + iteminfos[i].id + \"'></div>\";\n"
    "    // Create hash for mapping from ids to events\n"
    "    itemevents[iteminfos[i].id] = { expanded : false, events: iteminfos[i].events };\n"
    "  };\n"
    "  document.getElementById( \"listdiv\" ).innerHTML = content;\n"
    "};\n"
    "</script>\n"
    "<h1>" + tr( "Report of missing items generated at " )
           + QDateTime::currentDateTime().date().toString() + "</h1>\n"
    "<div id='listdiv'/>\n</body></html>\n";

    // save the file
    QTextStream out(&file);
    out << report.join("");
    file.close();

    QMessageBox::information( this, tr( "File written" ),
                              tr( "Report of missing items was written to'%1'." ).arg( filename ) );
}
