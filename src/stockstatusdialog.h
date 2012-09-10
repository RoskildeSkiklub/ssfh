#ifndef STOCKSTATUSDIALOG_H
#define STOCKSTATUSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class StockStatusDialog;
}

class StockStatusDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit StockStatusDialog(QWidget *parent = 0);
    ~StockStatusDialog();
    
private slots:
    void on_input_print_pushButton_clicked();

    void on_input_close_pushButton_clicked();

    void on_input_from_dateEdit_editingFinished();

    void on_input_to_dateEdit_editingFinished();

private:
    Ui::StockStatusDialog *ui;

    /** \brief The model that holds the data about the items */
    QSqlQueryModel item_model;

    /** \brief The model that holds the data about the itemevents */
    QSqlQueryModel itemevents_model;

    /** \brief Update the item models, e.g. after from/to date change */
    void updateItemModel();
};

#endif // STOCKSTATUSDIALOG_H
