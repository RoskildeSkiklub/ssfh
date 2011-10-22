
// Qt
#include <QLabel>
#include <QSqlQuery>
#include <QVariant>
#include <QShowEvent>
#include <QMap>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>

// App ui
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rentdialog.h"
#include "returndialog.h"
#include "addnewitemdialog.h"
#include "viewhireditemsdialog.h"
#include "printchecklistdialog.h"
#include "massstatechangedialog.h"
#include "selectcontractdialog.h"
#include "checksetupdialog.h"
#include "swapdialog.h"
#include "reiddialog.h"

// App
#include "log.h"
#include "exception.h"
#include "utility.h"
#include "db_consts.h"
#include "posprinter.h"
#include "globals.h"


using namespace Log;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect to barcode command event
    connect( Globals::interceptor, SIGNAL(barcodeCommandScan(Globals::BarcodeCommands::Command)),
             this, SLOT(on_barcodeCommandScan(Globals::BarcodeCommands::Command)) );
    // And to failure to scan
    connect( Globals::interceptor, SIGNAL(magSwipeFailure()),
             this, SLOT(on_magSwipeFailure()) );


    // Set up the db label widget in the statusbard
    status_db_label = new QLabel(this);
    statusBar()->addWidget( status_db_label );
    // updateDbStatusDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::showEvent(QShowEvent *) {
    Logger log("void MainWindow::showEvent(QShowEvent *event)");
    updateDbStatusDisplay();
}

void MainWindow::on_barcodeCommandScan(const Globals::BarcodeCommands::Command &command) {
    Logger log("void MainWindow::on_barcodeCommandScan(const Globals::BarcodeCommands::Command &command)");
    if ( QApplication::activeWindow() != this ) {
        log.stream() << "Ignoring commandScan because window is not active";
        return;
    }

    if ( command.m_code == Globals::BarcodeCommands::OpenRentalWindow ) {
        showRentDialog();
        return;
    }
    if ( command.m_code == Globals::BarcodeCommands::OpenReturnWindow ) {
        showReturnDialog();
        return;
    }

}

void MainWindow::on_magSwipeFailure() {
    Logger log("void MainWindow::on_magSwipeFailure()");
    QMessageBox::warning( this, tr( "Card reader failure"),
                          tr( "Failure reading magnetic card data. "
                              "Either the card could not be read, or the card was of an unknown type." ) );
}

void MainWindow::updateDbStatusDisplay() const {
    Logger log("void MainWindow::updateDbStatusDisplay()");
    QMap<QString,QString> item_map;
    QSqlQuery query;

    // Contracts
    query_check_prepare( query , "select state, count(*) "
                         "from contracts "
                         "where state=:state1 "
                         "or state=:state2 "
                         "or state=:state3 "
                         "group by state "
                         "order by state ");
    query.bindValue(":state1", DB::Contract::State::active);
    query.bindValue(":state2", DB::Contract::State::booking);
    query.bindValue(":state3", DB::Contract::State::parked);
    query_check_exec( query );
    QString cres;
    while( query.next() ) {
        cres += QString( " %0:%1" )
                .arg( Contract::tr( query.value( 0 ).toString().toLatin1().constData() ) )
                .arg( query.value( 1 ).toLongLong() );
    }
    if ( !cres.isEmpty() ) {
        cres = tr( "<b>contracts:</b>") + cres + "<br/>";
    }
    // Items
    query_check_prepare( query, "select type, rentalgroup, state, count(*) "
                         "from items "
                         "where state=:state1 "
                         "or state=:state2 "
                         "or state=:state3 "
                         "group by type, rentalgroup, state "
                         "order by type, rentalgroup, state" );
    query.bindValue(":state1", DB::Item::State::in );
    query.bindValue(":state2", DB::Item::State::booked );
    query.bindValue(":state3", DB::Item::State::out );
    query_check_exec( query );
    while( query.next() ) {
        QString ikey = QString( "%0/%1" )
                       .arg( query.value( 0 ).toString() )
                       .arg( query.value( 1 ).toString() );


        if ( !item_map.contains( ikey ) ) {
            item_map.insert( ikey, QString( "<b>%0:</b>").arg( ikey ) );
        }
        item_map[ ikey ] = item_map[ ikey ]
                           + QString( " %0:%1")
                           .arg( Item::tr( query.value( 2 ).toString().toLatin1().constData() ) )
                           .arg( query.value( 3 ).toLongLong() );
    }
    status_db_label->setText( cres + QStringList( item_map.values() ).join( "  " ) );
}


void MainWindow::showRentDialog() const {
    Logger log( "void MainWindow::showRentalDialog() const" );
    log.stream() << "Displaying rentDialog";
    RentDialog rentDialog;
    rentDialog.exec();
    updateDbStatusDisplay();
}

void MainWindow::showSwapDialog() const {
    Logger log( "void MainWindow::showSwapDialog() const" );
    log.stream() << "Displaying swapDialog";
    SwapDialog swapDialog;
    swapDialog.exec();
    updateDbStatusDisplay();
}

void MainWindow::showReturnDialog() const {
    Logger log( "void MainWindow::showReturnDialog() const" );
    log.stream() << "Displaying returnDialog";
    ReturnDialog returnDialog;
    returnDialog.exec();
    updateDbStatusDisplay();
}

void MainWindow::showAddItemsDialog() const {
    Logger log("void MainWindow::showAddItemsDialog() const");
    log.stream() << "Displaying addItem dialog";
    AddNewItemDialog addNewItemDialog;
    addNewItemDialog.exec();
    updateDbStatusDisplay();
}

void MainWindow::showHiredItemsDialog() const {
    Logger log("void MainWindow::showHiredItemsDialog() const");
    log.stream() << "Displaying HiredItems dialog";
    ViewHiredItemsDialog vhdi;
    vhdi.exec();
    updateDbStatusDisplay();
}

void MainWindow::showPrintChecklistDialog() const {
    Logger log("void MainWindow::showPrintChecklistDialog() const");
    log.stream() << "Displaying PrintChecklist dialog";
    PrintChecklistDialog pcld;
    pcld.exec();;
    updateDbStatusDisplay();
}

void MainWindow::showMassStateChangeDialog() const {
    Logger log("void MainWindow::showMassStateChangeDialog() const");
    log.stream() << "Displaying MassStateChange dialog";
    MassStateChangeDialog mscd;
    mscd.exec();
    updateDbStatusDisplay();
}

void MainWindow::showContractsDialog(const QString &state) const {
    Logger log("void MainWindow::showContractsDialog(const QString &state) const");
    log.stream() << "Wants to show contracts of type '" << state << "'";
    if ( state != DB::Contract::State::parked ) {
        throw Exception( Errors::InternalError )
                << (log.stream( error ) << "Can only show contracts with state 'parked'");
    }
    SelectContractDialog scd( state );
    // TODO: React to this stuff here...
    if ( QDialog::Accepted == scd.exec() && scd.getContractId() != -1 ) {
        log.stream() << "Displaying rentDialog";
        RentDialog rentDialog;
        rentDialog.useParkedContract( scd.getContractId() );
        rentDialog.exec();
    }
    updateDbStatusDisplay();
}

// TODO: I am not entirely sure this should go here. For now...
using namespace Globals::BarcodeCommands;
void MainWindow::doPrintCommandSheet() const {
    Logger log("void MainWindow::doPrintCommandSheet() const");
    // List of commands
    QList<Command> commands;
    commands << Command( OperationDone, tr( "Done" ) )
            // TODO: This should match some kind of configuration/db lookup, when POS is added.
            // Perhaps use the configuration table (*doh*)

            << Command( OpenRentalWindow, tr( "Open Rental Window") )
            << Command( OpenReturnWindow, tr( "Open Return Window" ) );
    /*

            << Command( ContractAddIndividual, tr( "Add Adult to Contract" ), tr( "Adult" ) )
            << Command( ContractAddIndividual, tr( "Add Child to Contract" ), tr( "Child" ) )
            << Command( ContractRemoveIndividual, tr( "Remove Adult from Contract" ), tr( "Adult" ) )
            << Command( ContractRemoveIndividual, tr( "Remove Child from Contract" ), tr( "Child" ) )
            << Command( ContractAddLiftCard, tr( "Add Adult Lift Card" ), tr( "Adult" ) )
            << Command( ContractAddLiftCard, tr( "Add Child Lift Card" ), tr( "Child" ) )
            << Command( ContractRemoveLiftCard, tr( "Remove Adult Lift Card" ), tr( "Adult" ) )
            << Command( ContractRemoveLiftCard, tr( "Remove Child Lift Card" ), tr( "Child" ) ) */

    if ( Globals::checkPosPrinter() ) {
        Pos::Printer & posp( Globals::getPosPrinter() );
        posp.startReceipt();
        posp.setFontSize(1, 2);
        posp << Pos::center << tr("List of Barcode Commands");
        posp << Pos::hr;
        QList<Command>::const_iterator i;
        for ( i = commands.begin(); i != commands.end(); ++i ) {
            posp << Pos::center << i->m_label;
            posp << Pos::center;
            if ( i->m_param.isEmpty() ) {
                posp << Pos::Barcode( QString("C%0").arg( i->m_code ) );
            } else {
                posp << Pos::Barcode( QString("C%0 %1").arg( i->m_code ).arg( i->m_param ).toUpper() );
            }
            posp << Pos::hr;
        }
        posp.endReceipt();
        statusBar()->showMessage( tr( "Command List printed" ), 5000 );;
        QMessageBox::information( 0, tr( "Command List Printed"),
                                  tr( "Printed Command List to printer.") );
    }
}

////////////////////////////////////////////////////////////////////

void MainWindow::on_action_RentItems_triggered() {
    Logger log("void MainWindow::on_action_RentItems_triggered()");
    showRentDialog();
}

void MainWindow::on_action_ReturnItems_triggered() {
    Logger log("void MainWindow::on_action_ReturnItems_triggered()");
    showReturnDialog();
}

void MainWindow::on_action_AddItems_triggered() {
    Logger log("void MainWindow::on_action_AddItems_triggered()");
    showAddItemsDialog();
}

void MainWindow::on_input_rent_pushButton_clicked() {
    Logger log("void MainWindow::on_input_rent_pushButton_clicked()");
    showRentDialog();
}

void MainWindow::on_input_return_pushButton_clicked() {
    Logger log("void MainWindow::on_input_return_pushButton_clicked()");
    showReturnDialog();
}

void MainWindow::on_action_HiredItems_triggered()
{
    Logger log("void MainWindow::on_action_HiredItems_triggered()");
    showHiredItemsDialog();
}

void MainWindow::on_actionPrint_Checklist_triggered()
{
    Logger log("void MainWindow::on_actionPrint_Checklist_triggered()");
    showPrintChecklistDialog();
}

void MainWindow::on_action_Mass_State_Change_triggered()
{
    Logger log("void MainWindow::on_action_Mass_State_Change_triggered()");
    showMassStateChangeDialog();
}

void MainWindow::on_action_Print_Command_Sheet_triggered()
{
    Logger log("void MainWindow::on_action_Print_Command_Sheet_triggered()");
    doPrintCommandSheet();
}

void MainWindow::on_action_About_triggered()
{
    Logger log("void MainWindow::on_action_About_triggered()");
    QMessageBox::information( this, tr("About SnowStuffForHire"),
                              tr( "<p>Version: %0</p><p>Created by Mads Bondo Dydensborg</p>"
                                  "<p>Mail: mads@dydensborg.dk</p><p>Phone: 29 84 00 42</p>")
                              .arg( VERSION ) );
}

void MainWindow::on_action_Parked_Contracts_triggered()
{
    Logger log("void MainWindow::on_action_Parked_Contracts_triggered()");
    showContractsDialog( DB::Contract::State::parked );
}

void MainWindow::on_action_Exit_triggered()
{
    if ( QMessageBox::Yes == QMessageBox::question( this, tr( "Exit application?"),
                                                    tr( "Are you sure you wish to exit?"),
                                                    QMessageBox::Yes | QMessageBox::No ) ) {
        close();
    }
}

void MainWindow::on_action_Check_Setup_triggered()
{
    Logger log("void MainWindow::on_action_Check_Setup_triggered()");
    CheckSetupDialog csd;
    csd.exec();

}

void MainWindow::on_input_swap_pushButton_clicked()
{
    Logger log("void MainWindow::on_input_swap_pushButton_clicked()");
    showSwapDialog();
}
