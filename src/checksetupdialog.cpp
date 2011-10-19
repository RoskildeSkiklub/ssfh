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
