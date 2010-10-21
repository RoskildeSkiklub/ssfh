
// App ui
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rentdialog.h"
#include "addnewitemdialog.h"

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

void MainWindow::on_rentButton_clicked() {
    Logger log( "void MainWindow::on_rentButton_clicked()" );
    log.stream() << "Displaying rentDialog";
    RentDialog rentDialog;
    // TODO: Protect here?
    rentDialog.exec();
}

void MainWindow::on_inventoryButton_clicked()
{
    Logger log( "MainWindow::on_inventoryButton_clicked()" );
    log.stream() << "Displaying addItem dialog";
    PROTECT_BLOCK(
        AddNewItemDialog addNewItemDialog;
        // TODO: Protect here?
        addNewItemDialog.exec();
        );
}
