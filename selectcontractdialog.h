#ifndef SELECTCONTRACTDIALOG_H
#define SELECTCONTRACTDIALOG_H

// Qt
#include <QDialog>
#include <QString>
#include <QSqlQueryModel>


namespace Ui {
    class SelectContractDialog;
}

class SelectContractDialog : public QDialog
{
    Q_OBJECT
    QString m_state; // The state of the contracts to show
    qlonglong m_contract_id;

public:
    explicit SelectContractDialog( const QString & state, QWidget *parent = 0);
    ~SelectContractDialog();

    /** \brief Get the id of the selected contract.
      * \return The id of the selectec contract, or -1 if no contract selected */
    qlonglong getContractId() const {  return m_contract_id; }

private:
    /** \brief The model that holds the data about the contracts */
    QSqlQueryModel model;

    Ui::SelectContractDialog *ui;

private slots:
    void on_input_contract_tableView_activated(QModelIndex index);
};

#endif // SELECTCONTRACTDIALOG_H
