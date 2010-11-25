#ifndef PLAINTEXTEDITDIALOG_H
#define PLAINTEXTEDITDIALOG_H

// Qt
#include <QDialog>
#include <QString>
namespace Ui {
    class PlainTextEditDialog;
}

class PlainTextEditDialog : public QDialog
{
    Q_OBJECT

public:

    /** \brief Constructor with all the needed settings
      * \param title The dialog title
      * \param heading The heading presented before the plaintextedit
      * \param text The initial text for the plain text edit
      * \param parent As usual */
    explicit PlainTextEditDialog(const QString & title, const QString & heading,
                                 const QString & text, QWidget * parent = 0);
    ~PlainTextEditDialog();

    /** \brief Get the text back from the edit
      * \return The text in the edit */
    QString getText() const;

private:
    Ui::PlainTextEditDialog *ui;
};

#endif // PLAINTEXTEDITDIALOG_H
