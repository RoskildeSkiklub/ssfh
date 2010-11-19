#ifndef MASSSTATECHANGEDIALOG_H
#define MASSSTATECHANGEDIALOG_H

#include <QDialog>

namespace Ui {
    class MassStateChangeDialog;
}

class MassStateChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MassStateChangeDialog(QWidget *parent = 0);
    ~MassStateChangeDialog();

private:
    Ui::MassStateChangeDialog *ui;
};

#endif // MASSSTATECHANGEDIALOG_H
