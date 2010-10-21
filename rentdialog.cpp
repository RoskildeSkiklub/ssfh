#include "rentdialog.h"
#include "ui_rentdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>

// App
#include "globals.h"
#include "log.h"
#include "exception.h"
#include "hirerdialog.h"
#include "dksundhedskort.h"

using namespace Log;

RentDialog::RentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RentDialog)
{
    Logger log( "RentDialog::RentDialog(QWidget *parent)");
    ui->setupUi(this);
    log.stream() << "Starting transaction";
    // TODO: Control on this... Can't remember what I wanted to do with this, really.....
    if ( ! QSqlDatabase::database().transaction() ) {
        log.stream( error ) << "Unable to start transaction";
    }
    //connect( ui->addButton, SIGNAL(clicked()), this, SLOT(on_addButton_clicked()));
    connect( Globals::interceptor, SIGNAL(barcodeScan(QString)), this, SLOT(add_item(QString)));
    connect( Globals::interceptor, SIGNAL( magSwipe(DKSundhedskort)), this, SLOT( set_hirer(DKSundhedskort) ) );
}

RentDialog::~RentDialog()
{
    delete ui;
}

void RentDialog::on_addButton_clicked() {
    Logger log( "void RentDialog::on_addButton_clicked()" );
    add_item( ui->lineEdit_item->text() );
}

///////////////////////////////////////////
// SET HIRER
void RentDialog::set_hirer (const DKSundhedskort &dsk) {
    Logger log( "void RentDialog::set_hirer (const DKSundhedskort &dsk)" );
    if ( try_set_hirer( dsk ) ) {
            ui->textEdit_hirer->setText( contract.toHtml() );
    }
}

bool RentDialog::try_set_hirer(const DKSundhedskort &dsk) {
    Logger log( "bool RentDialog::try_set_hirer(const DKSundhedskort &dsk)" );
    // TODO: Probably not set, if already set, stuff like that.
    // PROTECT_BLOCK( )
    // QMessageBox::information( this, "You scanned a hirer", dsk.name );
    return true;
}

///////////////////////////////////////////
// ADD ITEM

// Can be used as target for barcode scanner signals.
void RentDialog::add_item( const QString & item_id ) {
    Logger log( "void RentDialog::add_item( const QString & item_id )" );
    if ( try_add_item( item_id ) ) {
        ui->lineEdit_item->setText("");
        ui->textEdit_items->setText( contract.toHtml() );
    }
}

bool RentDialog::try_add_item(const QString &item_id) {
    Logger log( "bool RentDialog::try_add_item(const QString &item_id)" );
    // This may have to be changed, if/when we want to react smarter to errors such as duplicating entries.
    PROTECT_BLOCK(
            contract.add_item( item_id );
            return true;
    );
    return false;
}


void RentDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void RentDialog::on_pushButton_clicked(bool checked)
{
    Logger log( "void RentDialog::on_pushButton_clicked(bool checked)" );
    log.stream() << "Displaying HirerDialog";
    HirerDialog hirerDialog;
    if ( QDialog::Accepted == hirerDialog.exec() ) {
        log.stream() << "Getting hirer from hirerDialog";
        log.stream() << "Hirer is " << hirerDialog.getHirer().m_firstName;
        QMessageBox::information( this, "Got Hirer", QString( "Got this hirer: %1").arg( hirerDialog.getHirer().m_firstName ) );
        // TODO FIX ME UP
        contract.m_hirer = hirerDialog.getHirer();
    } else {
        log.stream() << "Did not get any hirer from hirerDialog";
    }
    // TODO: Read values back, etc.
}
