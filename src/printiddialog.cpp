#include "printiddialog.h"
#include "ui_printiddialog.h"

#include <QMessageBox>
#include <QRegExp>
#include <QPushButton>

// app
#include "log.h"
#include "globals.h"
#include "posprinter.h"

using namespace Log;

PrintIdDialog::PrintIdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintIdDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
}

PrintIdDialog::~PrintIdDialog()
{
    delete ui;
}

void PrintIdDialog::on_buttonBox_accepted()
{
    Logger log( "void PrintIdDialog::on_buttonBox_accepted()" );
    if ( doPrint() ) {
        QMessageBox::information( this, tr( "Item Ids printed"),
                                 tr( "The item ids have been printed as barcodes") );
        close();
    }
}

void PrintIdDialog::on_buttonBox_rejected()
{
    Logger log("void PrintIdDialog::on_buttonBox_rejected()");
    close();
}

bool PrintIdDialog::doPrint() const {
    Logger log("void PrintIdDialog::doPrint() const");

    if ( Globals::checkPosPrinter() ) {
        QStringList lines
                = ui->input_itemIds_plainTextEdit->toPlainText().split( QRegExp( "\n") );

        if ( !lines.empty() ) {
            log.stream() << "Starting to print item ids";
            Pos::Printer & posp( Globals::getPosPrinter() );
            posp.setFontSize();
            posp.startReceipt();
            QString line;
            foreach ( line, lines ) {
                posp << Pos::center << Pos::Barcode( line ) << Pos::hr;
            }
            posp.endReceipt();
            return true;
        } else {
            log.stream() << "No lines to print";
        }
    }
    return false;
}

void PrintIdDialog::on_input_itemIds_plainTextEdit_textChanged()
{
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ! ui->input_itemIds_plainTextEdit->toPlainText().isEmpty() );
}
