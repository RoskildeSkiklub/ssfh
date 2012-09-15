#ifndef PRINTCHECKLISTDIALOG_H
#define PRINTCHECKLISTDIALOG_H

#include <QDialog>
#include <QStringList>

class QSqlQuery;

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

    /** \brief Holds the current sort criteria
      *
      * This is in database names. */
    QStringList sortList;

    /** \brief Actual print method
      *
      * This does the actual printing, based on settings in the dialog */
    void doPrint();

    static void doItemSplitPrint( const QString & header1, const QString & header2,
                                  QSqlQuery query, bool includeBarCodes, int sortLevel, int maxItemsPrPage );

    /** \brief Update the count of items in a current state */
    void updateItemsCount();

    /** \brief Update the display of the sort criteria */
    void updateSortDisplay();

private slots:
    void on_input_close_pushButton_clicked();
    void on_input_removeSortCriteria_pushButton_clicked();
    void on_input_addSortCriteria_pushButton_clicked();
    void on_input_sortBy_comboBox_currentIndexChanged(int index);
    void on_input_itemState_comboBox_currentIndexChanged(int index);
    void on_input_print_pushButton_clicked();
};

#endif // PRINTCHECKLISTDIALOG_H
