#ifndef STOCKSTATUSDIALOG_H
#define STOCKSTATUSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QSqlQuery>

class QDate;

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

    void on_output_items_tableView_activated(const QModelIndex &index);

    void on_input_savereport_pushButton_clicked();

private:
    Ui::StockStatusDialog *ui;

    /** \brief Create a query to get the items we are interessted in.
      * \param columnClause The columns to select, e.g. "type, id" or "id, type, size, ..:"
      * \param from The date from we are interessted in, inclusive
      * \param to The date until we are interessted in, incluive.
      * \param orderClause Order of ids, e.g. "order by id" or "order by id, type"
      * \returns A query object ready to run exec on. */
    static QSqlQuery createItemQuery( const QString & columnClause,
                                      const QDate & fromdate,
                                      const QDate & todate,
                                      const QString & orderClause );

    /** \brief The model that holds the data about the items */
    QSqlQueryModel item_model;

    /** \brief The model that holds the data about the itemevents */
    QSqlQueryModel itemevents_model;

    /** \brief Update the item models, e.g. after from/to date change */
    void updateItemModel();
};

#endif // STOCKSTATUSDIALOG_H
