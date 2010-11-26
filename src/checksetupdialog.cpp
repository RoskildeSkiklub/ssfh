#include "checksetupdialog.h"
#include "ui_checksetupdialog.h"

// App
#include "log.h"
#include "qusb.h"
#include "utility.h"

using namespace Log;

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
    if ( QUSB::isDevicePresent( 0x0403, 0x6001 ) ) {
        ui->output_customerDisplayStatus_label->setText( tr("Present") );
    } else {
        ui->output_customerDisplayStatus_label->setText( tr( "Missing" ) );
    }
}

void CheckSetupDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    TODO("Figure out retry.")
}
