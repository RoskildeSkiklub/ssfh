#ifndef PERFORMSTATUSDIALOG_H
#define PERFORMSTATUSDIALOG_H

#include <QDialog>

namespace Ui {
class PerformStatusDialog;
}

class PerformStatusDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PerformStatusDialog(QWidget *parent = 0);
    ~PerformStatusDialog();
    
public slots:
    void scan_item( const QString & item_id );

signals:
    void item_scanned();

private slots:

    void on_input_close_pushButton_clicked();

private:
    Ui::PerformStatusDialog *ui;
};

#endif // PERFORMSTATUSDIALOG_H
