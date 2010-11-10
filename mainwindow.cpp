
// App ui
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rentdialog.h"
#include "returndialog.h"
#include "addnewitemdialog.h"
#include "viewhireditemsdialog.h"

// App
#include "log.h"
#include "exception.h"


using namespace Log;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // This is not needed,
    // because we use the on_*_clicked pattern?
    // connect( ui->rentButton, SIGNAL(clicked()), this, SLOT(on_rentButton_clicked()) );
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

void MainWindow::showRentDialog() const {
    Logger log( "void MainWindow::showRentalDialog() const" );
    log.stream() << "Displaying rentDialog";
    RentDialog rentDialog;
    rentDialog.exec();
}

void MainWindow::showReturnDialog() const {
    Logger log( "void MainWindow::showReturnDialog() const" );
    log.stream() << "Displaying returnDialog";
    ReturnDialog returnDialog;
    returnDialog.exec();
}

void MainWindow::showAddItemsDialog() const {
    Logger log("void MainWindow::showAddItemsDialog() const");
    log.stream() << "Displaying addItem dialog";
    AddNewItemDialog addNewItemDialog;
    addNewItemDialog.exec();
}

void MainWindow::showHiredItemsDialog() const {
    Logger log("void MainWindow::showHiredItemsDialog() const");
    log.stream() << "Displaying HiredItems dialog";
    ViewHiredItemsDialog vhdi;
    vhdi.exec();
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
