#ifndef STOCKDIALOG_H
#define STOCKDIALOG_H

#include <QDialog>

namespace Ui {
    class StockDialog;
}

class StockDialog : public QDialog {
    Q_OBJECT
public:
    StockDialog(QWidget *parent = 0);
    ~StockDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::StockDialog *ui;
};

#endif // STOCKDIALOG_H
