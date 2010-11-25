#include "stockdialog.h"
#include "ui_stockdialog.h"

StockDialog::StockDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StockDialog)
{
    ui->setupUi(this);
}

StockDialog::~StockDialog()
{
    delete ui;
}

void StockDialog::changeEvent(QEvent *e)
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
