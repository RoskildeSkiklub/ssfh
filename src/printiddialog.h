#ifndef PRINTIDDIALOG_H
#define PRINTIDDIALOG_H

#include <QDialog>

namespace Ui {
    class PrintIdDialog;
}

class PrintIdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintIdDialog(QWidget *parent = 0);
    ~PrintIdDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::PrintIdDialog *ui;
};

#endif // PRINTIDDIALOG_H
