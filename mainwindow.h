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
    void on_action_Print_Command_Sheet_triggered();
    void on_action_Mass_State_Change_triggered();
    void on_actionPrint_Checklist_triggered();
    void on_action_HiredItems_triggered();
    void on_action_AddItems_triggered();
    void on_action_ReturnItems_triggered();
    void on_action_RentItems_triggered();
    void on_input_return_pushButton_clicked();
    void on_input_rent_pushButton_clicked();

public slots:
    void on_barcodeCommandScan( const Globals::BarcodeCommands::Command & command );
};

#endif // MAINWINDOW_H
