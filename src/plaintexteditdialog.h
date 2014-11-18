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
