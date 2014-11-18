//
// Snow Stuff For Hire - A POS-like system designed for very quick rental
// operations at small ski resorts
// Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
//
// This file is part of Snow Stuff For Hire.
//
// Snow Stuff For Hire is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Snow Stuff For Hire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
//

#include "dbmaintenance.h"

// Qt
#include <QString>
#include <QSqlQuery>
#include <QVariant>

// App
#include "item.h"
#include "contract.h"
#include "db_consts.h"
#include "log.h"
#include "exception.h"
#include "utility.h"
#include "contract.h"

using namespace Log;

DbMaintenance::DbMaintenance()
{
}

Item DbMaintenance::itemForceState(const QString &item_id, const QString &state) const {
    Logger log("DbMaintenance::itemForceState(const QString &item_id, const QString &state) const");
    log.stream() << "item_id : '" << item_id
            << "', state: '" << state << "'";
    if ( state == DB::Item::State::booked
         || state == DB::Item::State::out ) {
        throw Exception( Errors::DbMaintenanceForceStateOnIllegalState )
                << ( log.stream( error )
                     << "Illegal state '" << state << "' passed as argument to DbMaintenance::itemForceState");
    }
    // Force the state change of any items
    Item item( Item::db_load( item_id ) );
    item.db_forceState( state, "DbMaintenance::itemForceState" );
    // Update any contracts that points at this item.
    // Note, that for all, it is "returned", that is the new state.
    //
    // TODO: If the item is booked, the booking disappears. Tough luck...
    QSqlQuery query;
    query_check_prepare( query, "select id  "
                         "from contractitems "
                         "where item_id = :item_id "
                         "and state != :state");
    query.bindValue( ":item_id", item_id );
    query.bindValue( ":state", DB::ContractItem::State::returned );
    query_check_exec( query );
    while( query.next() ) {
        qlonglong id = query.value( 0 ).toLongLong();
        log.stream( info ) << "Found active contractitem for item with id '"
                << item_id << "'. ContractItem has id: '" << id << "'. Setting to 'returned'";
        // Just change the contractItem, as the item has been changed correctly.
        ContractItem ci( ContractItem::db_load( id ) );
        ci.db_update_state( DB::ContractItem::State::returned );
        // Now, check the contract that the ci points at
        log.stream() << "ContractItem set to returned. Now checking Contract with id '"
                << ci.getContractId() << "' for closing.";
        Contract contract( Contract::db_load( ci.getContractId() ) );
        // TODO If contract is booking - this goes wild...
        if ( contract.getState() == DB::Contract::State::active
             && !contract.hasReturnableItems() ) {
            log.stream() << "Can close contract with id '"
                    << ci.getContractId() << "'";
            contract.close();
        }
    }
    return item;
}
