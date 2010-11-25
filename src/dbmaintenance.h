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
