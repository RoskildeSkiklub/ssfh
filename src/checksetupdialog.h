#ifndef CHECKSETUPDIALOG_H
#define CHECKSETUPDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
    class CheckSetupDialog;
}

class CheckSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckSetupDialog(QWidget *parent = 0);
    ~CheckSetupDialog();

private:
    Ui::CheckSetupDialog *ui;

    /** \brief Update display of status */
    void update();


private slots:
    void on_buttonBox_clicked(QAbstractButton* button);
};

#endif // CHECKSETUPDIALOG_H
