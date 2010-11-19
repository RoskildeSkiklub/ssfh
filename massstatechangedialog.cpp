#include "massstatechangedialog.h"
#include "ui_massstatechangedialog.h"

MassStateChangeDialog::MassStateChangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MassStateChangeDialog)
{
    ui->setupUi(this);
}

MassStateChangeDialog::~MassStateChangeDialog()
{
    delete ui;
}
