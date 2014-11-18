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

#include "printiddialog.h"
#include "ui_printiddialog.h"

#include <QMessageBox>
#include <QRegExp>
#include <QPushButton>

// app
#include "log.h"
#include "globals.h"
#include "posprinter.h"
#include "printerhelpers.h"

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
    QStringList lines
            = ui->input_itemIds_plainTextEdit->toPlainText().split( QRegExp( "\n") );
    if ( lines.empty() ) {
        log.stream() << "No lines to print";
        return false;
    }
    return PrinterHelpers::printStringsAsBarcodes( "List of ids", lines, "" );
}

void PrintIdDialog::on_input_itemIds_plainTextEdit_textChanged()
{
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ! ui->input_itemIds_plainTextEdit->toPlainText().isEmpty() );
}
