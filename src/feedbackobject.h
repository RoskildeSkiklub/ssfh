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

#ifndef FEEDBACKOBJECT_H
#define FEEDBACKOBJECT_H

// Qt
#include <QObject>
#include <QMap>

// TODO: Migrate to QMultiMedia something. Right now, just eliminated everything, and
// calls mplayer... 

/** \brief Object to provide audio feedback on events */
class FeedbackObject : public QObject
{
    Q_OBJECT

public:
    explicit FeedbackObject(QObject *parent = 0);
    ~FeedbackObject();


    /** \brief The different types of event that gives feedback */
    typedef enum {
        ItemAdded = 0,
        ItemReturned = 1,
        HirerSet = 2,
        ItemScanned = 3
    } EventType;

public slots:
    /** Feedback on specific event
     * \param type The type of the event */
    void eventTriggered( const EventType & type ) const;

    // The rest here is actually deprecated.

    /** \brief The sound of an item added to a contract */
    void itemAdded() const;
    /** \brief The sound of an item returned on a contract */
    void itemReturned() const;
    /** \brief The sound of an hirer set on a contract */
    void hirerSet() const;
    /** \brief The sound of an item scanned (general version of added, really) */
    void itemScanned() const;

private:
    // Ressourcename *without* sound
    QString addSound( const QString & ressourceName,
                      const QString & targetDir );

    // Map eventtypes to filenames
    QMap<EventType,QString> eventToFilename;
};

#endif // FEEDBACKOBJECT_H
