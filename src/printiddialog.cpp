#include "printiddialog.h"
#include "ui_printiddialog.h"

#include <QMessageBox>

// app
#include "log.h"

using namespace Log;

PrintIdDialog::PrintIdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintIdDialog)
{
    ui->setupUi(this);
}

PrintIdDialog::~PrintIdDialog()
{
    delete ui;
}

void PrintIdDialog::on_buttonBox_accepted()
{
    Logger log( "void PrintIdDialog::on_buttonBox_accepted()" );
    QMessageBox::information( this, tr( "Item Ids printed"),
                             tr( "The item ids have been printed as barcodes") );
}

void PrintIdDialog::on_buttonBox_rejected()
{
    Logger log("void PrintIdDialog::on_buttonBox_rejected()");
    close();
}
