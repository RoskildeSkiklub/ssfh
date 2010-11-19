#include "massstatechangedialog.h"
#include "ui_massstatechangedialog.h"

// Qt
#include <QString>
#include <QMessageBox>

// App
#include "log.h"
#include "db_consts.h"
#include "globals.h"
#include "item.h"
#include "dbmaintenance.h"

using namespace Log;

MassStateChangeDialog::MassStateChangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MassStateChangeDialog)
{
    ui->setupUi(this);
    // Fill combobox with possibly state values. Note,
    // we do not allow setting to states that are part of contracts
    ui->input_state_comboBox->
            addItem( Item::tr( DB::Item::State::in.toLatin1().constData() ),
                     DB::Item::State::in  );
    ui->input_state_comboBox->
            addItem( Item::tr( DB::Item::State::maintenance.toLatin1().constData() ),
                     DB::Item::State::maintenance );
    ui->input_state_comboBox->
            addItem( Item::tr( DB::Item::State::lost.toLatin1().constData() ),
                     DB::Item::State::lost  );
    ui->input_state_comboBox->
            addItem( Item::tr( DB::Item::State::discarded.toLatin1().constData() ),
                     DB::Item::State::discarded  );
    ui->input_state_comboBox->setCurrentIndex( 0 );

    // Set up signals
    connect( Globals::interceptor, SIGNAL(barcodeScan(QString)),
             this, SLOT(update_item(QString)));

}

MassStateChangeDialog::~MassStateChangeDialog()
{
    delete ui;
}

void MassStateChangeDialog::update_item(const QString &item_id) {
    Logger log("void MassStateChangeDialog::update_item(const QString &item_id)");
    int csi = ui->input_state_comboBox->currentIndex();
    if ( csi < 0 ) {
        log.stream(warn) << "Index of state < 0, ignoring";
    }
    DbMaintenance dbmain;
    Item item( dbmain.itemForceState( item_id, ui->input_state_comboBox->itemData( csi ).toString() ) );
    ui->output_log_textBrowser->
            setHtml( QString("<p>%0: <em>%1</em>").arg( item.toRentalHtml() )
                     .arg( Item::tr( item.getState().toLocal8Bit().constData() ) )
                     + ui->output_log_textBrowser->toHtml() );
}

void MassStateChangeDialog::on_input_done_pushButton_clicked()
{
    Logger log("void MassStateChangeDialog::on_input_done_pushButton_clicked()");
    close();
}

void MassStateChangeDialog::on_input_doIt_pushButton_clicked() {
    Logger log( "void MassStateChangeDialog::on_input_doIt_pushButton_clicked()");
    if ( ui->input_item_lineEdit->text().isEmpty() ) {
        log.stream(warn) << "Internal: called, but empty item_lineEdit";
    } else {
        update_item( ui->input_item_lineEdit->text() );
        ui->input_item_lineEdit->clear();
        ui->input_item_lineEdit->setFocus();
    }
}
