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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>
class QLabel;
class QShowEvent;

// App
class DKSundhedskort;
namespace Globals {
    namespace BarcodeCommands {
        class Command;
    }
};

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

    void showRentDialog() const;
    void showReturnDialog() const;
    void showAddItemsDialog() const;
    void showHiredItemsDialog() const;
    void showPrintChecklistDialog() const;
    void showMassStateChangeDialog() const;
    void doPrintCommandSheet() const;
    void showContractsDialog( const QString & state ) const;
    void showSwapDialog() const;
    void showReIdDialog() const;
    void showPrintIdDialog() const;
    void showPerformStatusDialog() const;
    void showStockStatusDialog() const;
    void showTrackItemDialog() const;

    /** \brief Pointer to statusbar db label
      *
      * This points to the label used to display information about
      * the database in the statusbar */
    QLabel * status_db_label;

    /**  \brief Updates the display of the db status */
    void updateDbStatusDisplay() const;

    /** \brief Make sure display is updated from event */
    void showEvent ( QShowEvent * event );

private slots:
    void on_input_swap_pushButton_clicked();
    void on_action_Check_Setup_triggered();
    void on_action_Exit_triggered();
    void on_action_Parked_Contracts_triggered();
    void on_action_About_triggered();
    void on_action_Print_Command_Sheet_triggered();
    void on_action_Mass_State_Change_triggered();
    void on_actionPrint_Checklist_triggered();
    void on_action_HiredItems_triggered();
    void on_action_AddItems_triggered();
    void on_action_ReturnItems_triggered();
    void on_action_RentItems_triggered();
    void on_input_return_pushButton_clicked();
    void on_input_rent_pushButton_clicked();

    /** \brief Display a warning dialog */
    void on_magSwipeFailure();

    void on_action_Reid_items_triggered();

    void on_actionPrint_Item_Barcodes_triggered();

    void on_action_Active_Contracts_triggered();

    void on_actionPerform_status_triggered();

    void on_actionStock_status_triggered();

    void on_action_Track_Item_triggered();

public slots:
    void on_barcodeCommandScan( const Globals::BarcodeCommands::Command & command );
};

#endif // MAINWINDOW_H
