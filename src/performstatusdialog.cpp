#include "performstatusdialog.h"
#include "ui_performstatusdialog.h"

// app
#include "log.h"
//#include "globals.h"
//#include "utility.h"
//#include "db_consts.h"
//#include "item.h"

using namespace Log;

PerformStatusDialog::PerformStatusDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PerformStatusDialog)
{
    ui->setupUi(this);
}

PerformStatusDialog::~PerformStatusDialog()
{
    delete ui;
}

// Signals and slots.


void PerformStatusDialog::scan_item(const QString &item_id) {
    Logger log("PerformStatusDialog::scan_item(const QString &item_id)");
    // TODO: Fix such that an itemevent is made, and so on. Make sure to make an abort sound
    // if scanning an item not there.
    // The next line should only happen, if we know the item.
    ui->output_lastscanneditem_textBrowser->setText(item_id);
    // TODO: Update with description,
    // TODO: Update the recently scanned stuff
    ui->output_recentlyscanneditems_textBrowser->setText( item_id + "\n" + "noget" );
    // Emit the sound of an item scanned.
    emit item_scanned();
}



void PerformStatusDialog::on_input_close_pushButton_clicked() {
    Logger log("void PerformStatusDialog::on_input_close_pushButton_clicked()");
    close();
}
