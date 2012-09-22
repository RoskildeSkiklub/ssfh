#ifndef TRACKITEMDIALOG_H
#define TRACKITEMDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class TrackItemDialog;
}

class TrackItemDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TrackItemDialog(QWidget *parent = 0);
    ~TrackItemDialog();
    
public slots:
    void scan_item( const QString & item_id );

signals:
    void item_scanned();

private slots:
    void on_input_close_pushButton_clicked();

    void on_input_lookup_pushButton_clicked();

private:
    Ui::TrackItemDialog *ui;

    /** \brief Method to track an item, that is, fill the dialog with stuff */
    void doTrackItem();

    /** \brief Method to clear the contents of the dialog */
    void clear();

    /** \brief The model that holds the data about the itemevents */
    QSqlQueryModel itemevents_model;

    /** \brief Update the item models, e.g. after from/to date change */
    void updateItemeventsModel();
};

#endif // TRACKITEMDIALOG_H
