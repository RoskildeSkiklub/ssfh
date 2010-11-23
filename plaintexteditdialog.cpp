#include "plaintexteditdialog.h"
#include "ui_plaintexteditdialog.h"

#include <QString>

PlainTextEditDialog::PlainTextEditDialog(const QString &title, const QString &heading,
                                         const QString &text, QWidget *parent)
                                             : QDialog(parent),
    ui(new Ui::PlainTextEditDialog)
{
    ui->setupUi(this);
    setWindowTitle( title );
    ui->output_heading->setText( heading );
    ui->input_text->setPlainText( text );
}

PlainTextEditDialog::~PlainTextEditDialog()
{
    delete ui;
}

QString PlainTextEditDialog::getText() const {
    return ui->input_text->toPlainText();
}

