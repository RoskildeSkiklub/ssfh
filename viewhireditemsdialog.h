#ifndef VIEWHIREDITEMSDIALOG_H
#define VIEWHIREDITEMSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>


namespace Ui {
    class ViewHiredItemsDialog;
}

class ViewHiredItemsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewHiredItemsDialog(QWidget *parent = 0);
    ~ViewHiredItemsDialog();

private:
    Ui::ViewHiredItemsDialog *ui;

    /** \brief The model that holds the items data */
    QSqlQueryModel *model;



private slots:
    void on_item_tableView_activated(QModelIndex index);
};

#endif // VIEWHIREDITEMSDIALOG_H
