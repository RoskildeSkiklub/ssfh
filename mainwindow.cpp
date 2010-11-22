
// Qt
#include <QLabel>
#include <QSqlQuery>
#include <QVariant>
#include <QShowEvent>
#include <QMap>
#include <QString>

// App ui
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rentdialog.h"
#include "returndialog.h"
#include "addnewitemdialog.h"
#include "viewhireditemsdialog.h"
#include "printchecklistdialog.h"
#include "massstatechangedialog.h"

// App
#include "log.h"
#include "exception.h"
#include "utility.h"
#include "db_consts.h"


using namespace Log;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // This is not needed,
    // because we use the on_*_clicked pattern?
    // connect( ui->rentButton, SIGNAL(clicked()), this, SLOT(on_rentButton_clicked()) );

    // Set up the db label widget
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

void MainWindow::showEvent(QShowEvent *event) {
    Logger log("void MainWindow::showEvent(QShowEvent *event)");
    updateDbStatusDisplay();
}

void MainWindow::updateDbStatusDisplay() const {
    Logger log("void MainWindow::updateDbStatusDisplay()");
    QMap<QString,QString> item_map;
    QSqlQuery query;
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
    status_db_label->setText( QStringList( item_map.values() ).join( "  " ) );
}


void MainWindow::showRentDialog() const {
    Logger log( "void MainWindow::showRentalDialog() const" );
    log.stream() << "Displaying rentDialog";
    RentDialog rentDialog;
    rentDialog.exec();
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


/////

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
