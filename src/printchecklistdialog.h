/* * Snow Stuff For Hire - A POS-like system designed for very quick rental
 * operations at small ski resorts
 * Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
 *
 * This file is part of Snow Stuff For Hire.
 *
 * Snow Stuff For Hire is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Snow Stuff For Hire is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
 */

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
