#ifndef PRINTCHECKLISTDIALOG_H
#define PRINTCHECKLISTDIALOG_H

#include <QDialog>

namespace Ui {
    class PrintChecklistDialog;
}

class PrintChecklistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintChecklistDialog(QWidget *parent = 0);
    ~PrintChecklistDialog();

private:
    Ui::PrintChecklistDialog *ui;

private slots:
    void on_pushButton_clicked();
};

#endif // PRINTCHECKLISTDIALOG_H
