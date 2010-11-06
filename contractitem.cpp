#include "contractitem.h"

// Qt
#include <QSqlQuery>
#include <QVariant>

// app
#include "log.h"
#include "db_consts.h"
#include "utility.h"
#include "exception.h"

using namespace Log;
ContractItem::ContractItem() : m_id( -1 ) {
    Logger log("ContractItem::ContractItem() : m_id( -1 )");
}

ContractItem::ContractItem(qlonglong contract_id, const Item &item,
                           const QString &rentalgroup, qlonglong price,
                           const QString &note )
                               : m_id( -1 ), m_contract_id( contract_id ),
                               m_item( item ),
                               m_state( DB::ContractItem::State::booked ),
                               m_rentalgroup( rentalgroup ),
                               m_price( price ), m_note( note ) {
    Logger log("ContractItem::ContractItem(qlonglong contract_id, const Item &item, const QString &rentalgroup, qlonglong price, const QString &note)");
    log.stream()
            << "m_id: '" << m_id
            << "', m_contract_id: '" << m_contract_id
            << "', m_item_id: '" << m_item.getId()
            << "', m_state: '" << m_state
            << "', m_rentalgroup '" << m_rentalgroup
            << "', m_price '" << m_price
            << "', m_note '" << m_note << "'";
}

void ContractItem::setState(const QString &state) {
    Logger log("void ContractItem::setState(const QString &state)");
    log.stream() << "Setting state to '" << state << "'";
    m_state = state;
}

QString ContractItem::toRentalHtml() const {
    Logger log("QString ContractItem::toRentalHtml() const");
    return QString( "<tr><td>%0</td><td align=\"right\">%1</td></tr>\n" )
            .arg( m_item.toRentalHtml() )
            .arg( m_price );
}

void ContractItem::db_insert() {
    Logger log("void ContractItem::db_insert()");
    QSqlQuery query;
    query_check_prepare( query, "insert into contractitems "
                         "( contract_id, item_id, state, rentalgroup, price, note ) "
                         "values ( :contract_id, :item_id, :state, :rentalgroup, :price, :note )");
    query.bindValue( ":contract_id", m_contract_id );
    query.bindValue( ":item_id", m_item.getId() );
    query.bindValue( ":state", m_state );
    query.bindValue( ":rentalgroup", m_rentalgroup );
    query.bindValue( ":price", m_price );
    query.bindValue( ":note", m_note );
    query_check_exec( query );
    // Read back the id
    // Read back the last value
    query_check_prepare( query, "select last_insert_rowid() from contractitems" );
    query_check_exec( query );
    query_check_first( query );
    m_id = query.value( 0 ).toLongLong();
    log.stream( info ) << "Created ContractItem with id '" << m_id << "'";
}

void ContractItem::db_update_state( const QString & new_state ) {
    Logger log("void ContractItem::db_update_state( const QString & new_state )");
    QSqlQuery query;
    query_check_prepare( query, "update contractitems "
                         "set state = :state "
                         "where id = :id" );
    query.bindValue( ":state", new_state );
    query.bindValue( ":id", m_id );
    query_check_exec( query );
    setState( new_state );
}

ContractItem ContractItem::db_load(qlonglong id) {
    Logger log("void ContractItem::db_load(qlonglong id)");
    QSqlQuery query;
    query_check_prepare( query,
                         "select contract_id, item_id, state, rentalgroup, price, note "
                         "from contractitems "
                         "where id=:id" );
    query.bindValue( ":id", id );
    query_check_exec( query );
    try {
        query_check_first( query );
    }
    catch (const Exception & e) {
        if ( e.getStatusCode() == Errors::DBResultError ) {
            throw Exception( Errors::ContractItemDoesNotExist )
            << ( log.stream( error )
                 //! \todo Translate???
                 << "The contractitem with id '" << id << "' does not exist" );
        }
        throw;
    }
    // Get the Item
    Item item( Item::db_load( query.value( 1 ).toString() ) );
    log.stream() << "Got Item, setting up contractItem";
    ContractItem res;
    res.m_id          = id;
    res.m_contract_id = query.value( 0 ).toLongLong();
    res.m_item        = item; // Column 1
    res.m_state       = query.value( 2 ).toString();
    res.m_rentalgroup = query.value( 3 ).toString();
    res.m_price       = query.value( 4 ).toLongLong();
    res.m_note        = query.value( 5 ).toString();
    log.stream() << "ContractItem loaded from database: " << res.toString();
    return res;

}

QString ContractItem::toString() const {
    Logger log("QString ContractItem::toString() const");
    return QString( "id: '%0', contract_id: '%1', item_id: '%2', state: '%3', "
                    "rentalgroup: '%4', price: '%5', note: '%6'" )
            .arg( m_id ).arg( m_contract_id ).arg( m_item.getId() )
            .arg( m_state ).arg( m_rentalgroup ).arg( m_price ).arg( m_note );
}






