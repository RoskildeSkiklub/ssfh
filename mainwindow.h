#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void on_actionPrint_Checklist_triggered();
    void on_action_HiredItems_triggered();
    void on_action_AddItems_triggered();
    void on_action_ReturnItems_triggered();
    void on_action_RentItems_triggered();
    void on_input_return_pushButton_clicked();
    void on_input_rent_pushButton_clicked();
};

#endif // MAINWINDOW_H
