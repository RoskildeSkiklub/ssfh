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
    connect( Globals::interceptor, SIGNAL(barcodeItemScan(QString)),
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

void MassStateChangeDialog::on_input_close_pushButton_clicked()
{
    Logger log("void MassStateChangeDialog::on_input_close_pushButton_clicked()");
    close();
}
