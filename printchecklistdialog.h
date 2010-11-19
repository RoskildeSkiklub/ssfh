#ifndef PRINTCHECKLISTDIALOG_H
#define PRINTCHECKLISTDIALOG_H

#include <QDialog>
#include <QStringList>

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

    /** \brief Update the count of items in a current state */
    void updateItemsCount();

    /** \brief Update the display of the sort criteria */
    void updateSortDisplay();

private slots:
    void on_input_removeSortCriteria_pushButton_clicked();
    void on_input_addSortCriteria_pushButton_clicked();
    void on_input_sortBy_comboBox_currentIndexChanged(int index);
    void on_input_itemState_comboBox_currentIndexChanged(int index);
    void on_input_print_pushButton_clicked();
};

#endif // PRINTCHECKLISTDIALOG_H
