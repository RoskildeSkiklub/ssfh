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
    void on_buttonBox_accepted(); // Print
    void on_buttonBox_rejected(); // Close

    void on_input_itemIds_plainTextEdit_textChanged();

private:
    Ui::PrintIdDialog *ui;

    /** \brief Do the actual print */
    bool doPrint() const;
};

#endif // PRINTIDDIALOG_H
