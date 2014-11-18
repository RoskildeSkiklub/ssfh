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

#include "checksetupdialog.h"
#include "ui_checksetupdialog.h"

// App
#include "log.h"
#include "qusb.h"
#include "utility.h"

using namespace Log;
// TODO : Probably move this somewhere else
const Usb::Id Pertelian( 0x0403, 0x6001 );
const Usb::Id MagTek( 0x0801, 0x001 );
const Usb::Id BarcodeScanner( 0x080c, 0x0300 );
const Usb::Id PosPrinter( 0x067b, 0x2305 );


CheckSetupDialog::CheckSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CheckSetupDialog)
{
    ui->setupUi(this);
    update();
}

CheckSetupDialog::~CheckSetupDialog()
{
    delete ui;
}

void CheckSetupDialog::update() {
    Logger log("void CheckSetupDialog::update()");
    // TODO: At least const this...
    if ( Usb::QUSB::isDevicePresent( Pertelian ) ) {
        ui->output_customerDisplayStatus_label->setText( tr( "Present" ) );
    } else {
        ui->output_customerDisplayStatus_label->setText( tr( "Missing" ) );
    }


}

void CheckSetupDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    TODO("Figure out retry.");
}
