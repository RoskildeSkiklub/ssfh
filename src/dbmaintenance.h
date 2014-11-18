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

#ifndef DBMAINTENANCE_H
#define DBMAINTENANCE_H

// Qt
class QString;

// App
#include "item.h"

class DbMaintenance
{
public:
    DbMaintenance();

    /** \brief Sets an item to the state passed
      * \param item_id The id of the item to change state for
      * \param state The new state to set for the item
      * \return The newly changed item, as loaded from the db
      *
      * This changes the item to the state passed. It also updates
      * any contract or contractitems to reflect the state of the
      * item now. Additionally, it closes any contracts referring to the
      * item, that have no open item lines left.
      *
      * Note, only states that does not have anything to do with hiring are
      * allowed. That is, 'booking' and 'out' are not allowed as states.
      *
      * This method throws on errors. */
    Item itemForceState( const QString & item_id, const QString & state ) const;

};

#endif // DBMAINTENANCE_H
