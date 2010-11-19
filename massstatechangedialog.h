#ifndef MASSSTATECHANGEDIALOG_H
#define MASSSTATECHANGEDIALOG_H

/** \file
  * \brief The mass state change dialog is commonly used to return items that have not been properly returned */

// Qt
#include <QDialog>
class QString;

namespace Ui {
    class MassStateChangeDialog;
}

class MassStateChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MassStateChangeDialog(QWidget *parent = 0);
    ~MassStateChangeDialog();

private:
    Ui::MassStateChangeDialog *ui;

private slots:

    /** \brief Update the state of an item with the current selected state
      * \param item_id The id of the item to update */
    void on_input_doIt_pushButton_clicked();
    void update_item( const QString & item_id );

    void on_input_done_pushButton_clicked();
};

#endif // MASSSTATECHANGEDIALOG_H
