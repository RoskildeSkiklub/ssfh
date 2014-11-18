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
