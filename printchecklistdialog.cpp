#include "printchecklistdialog.h"
#include "ui_printchecklistdialog.h"

// Qt
#include <QSqlQuery>
#include <QMessageBox>
#include <QVariant>
#include <QString>

// app
#include "globals.h"
#include "utility.h"
#include "db_consts.h"
#include "item.h"

PrintChecklistDialog::PrintChecklistDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintChecklistDialog)
{
    ui->setupUi(this);
}

PrintChecklistDialog::~PrintChecklistDialog()
{
    delete ui;
}

void PrintChecklistDialog::on_pushButton_clicked()
{
    QSqlQuery query;
    query_check_prepare( query, "select id from items where state = :state ");
    query.bindValue( ":state", DB::Item::State::out );
    query_check_exec( query );
    if ( query.first() ) {
        if ( Globals::checkPosPrinter() ) {
            Pos::Printer & posp( Globals::getPosPrinter() );
            posp.startReceipt();
            posp << tr( "List of missing items" );
            posp << Pos::hr;
            int count = 0;
            do {
                posp << Pos::endl;
                QString item_id = query.value(0).toString();
                Item item( Item::db_load( item_id ) );
                posp << item.toReceiptString() << Pos::endl;
                posp << Pos::center << Pos::Barcode( item_id ) << Pos::hr;
                ++count;
            } while( query.next() && count < 10 );
            posp.endReceipt();
            QMessageBox::information( this, tr("Checklist printed"),
                                      tr( "Checklist printed" ) );
        }
    } else {
        QMessageBox::information( this, tr("No items found"),
                                  tr( "No missing items found") );
    }
}
