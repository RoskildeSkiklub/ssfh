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

#include "item.h"

// Qt
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDateTime>

// App
#include "db_consts.h"
#include "utility.h"
#include "log.h"
#include "exception.h"

using namespace Log;

Item::Item()
{
}

Item::Item(QString id, QString type, QString size, QString mark, QString model, QString year, QString condition, long price, QString rentalgroup, QString note)
    : m_id( id ), m_type( type ), m_size( size ), m_mark( mark ), m_model( model ), m_year( year ), m_condition( condition ), m_price( price ),
    m_rentalgroup( rentalgroup ), m_state( DB::Item::State::in ), m_note( note )
{
    Logger log( "Item::Item(QString id, QString type, QString size, QString mark, QString model, QString year, QString condition, long price, QString rentalgroup, QString note)" );
    log.stream() << "Item created: "
            << "id: '" << m_id << "', "
            << "type: '" << m_type << "', "
            << "size: '" << m_size << "', "
            << "mark: '" << m_mark << "', "
            << "model: '" << m_model << "', "
            << "year: '" << m_year << "', "
            << "condition: '" << m_condition << "', "
            << "price: '" << price << "', "
            << "rentalgroup: '" << m_rentalgroup << "', "
            << "state: '" << m_state << "', "
            << "note: '" << m_note << "'";
}

void Item::db_insert() {
    Logger log("void Item::db_insert()");
    QSqlQuery query;
    qlonglong price = m_price;
    query_check_prepare( query, "insert into items ( id, type, size, mark, model, year, condition, price, rentalgroup, state, note ) "
                         "values ( :id, :type, :size, :mark, :model, :year, :condition, :price, :rentalgroup, :state, :note )");
    query.bindValue( ":id", m_id );
    query.bindValue( ":type", m_type );
    query.bindValue( ":size", m_size );
    query.bindValue( ":mark", m_mark );
    query.bindValue( ":model", m_model );
    query.bindValue( ":year", m_year );
    query.bindValue( ":condition", m_condition );
    query.bindValue( ":price", price );
    query.bindValue( ":rentalgroup", m_rentalgroup );
    query.bindValue( ":state", m_state );
    query.bindValue( ":note", m_note );
    query_check_exec( query );
    log.stream( info ) << "New item inserted into db: "
            << "id: '" << m_id << "', "
            << "type: '" << m_type << "', "
            << "size: '" << m_size << "', "
            << "mark: '" << m_mark << "', "
            << "model: '" << m_model << "', "
            << "year: '" << m_year << "', "
            << "condition: '" << m_condition << "', "
            << "price: '" << price << "', "
            << "rentalgroup: '" << m_rentalgroup << "', "
            << "state: '" << m_state << "', "
            << "note: '" << m_note << "'";
    // Add a log line to the log
    log.stream() << "Adding log line to itemevents";
    addEventLine( DB::Item::Event::created, "(Item::db_insert())" );
}

Item Item::db_load(const QString &id) {
    Logger log("Item Item::db_load(const QString &id)");
    log.stream() << "Loading item with id '" << id << "'";
    // Locate the data about the items in the database
    QSqlQuery query;
    query_check_prepare( query, "select type, size, mark, model, year, condition, price, rentalgroup, state, note "
                                "from items "
                                "where id = :id" );
    query.bindValue( ":id", id );

    // Execute and move to first.
    query_check_exec( query );
    try {
        query_check_first( query );
    }
    catch (const Exception & e) {
        if ( e.getStatusCode() == Errors::DBResultError ) {
            throw Exception( Errors::ItemDoesNotExist )
            << ( log.stream( error )
                 //! \todo Translate???
                 << "The item with id '" << id << "' does not exist" );
        }
        throw;
    }

    // Load values
    Item res;
    res.m_id          = id;
    res.m_type        = query.value( 0 ).toString();
    res.m_size        = query.value( 1 ).toString();
    res.m_mark        = query.value( 2 ).toString();
    res.m_model       = query.value( 3 ).toString();
    res.m_year        = query.value( 4 ).toString();
    res.m_condition   = query.value( 5 ).toString();
    res.m_price       = query.value( 6 ).toInt();
    res.m_rentalgroup = query.value( 7 ).toString();
    res.m_state       = query.value( 8 ).toString();
    res.m_note        = query.value( 9 ).toString();
    log.stream() << "Item loaded from database: " << res.toString();
    return res;
}

Item Item::locate_and_book_in_db( const QString & id, qlonglong contract_id ) {
    Logger log( "Item locate_book_in_db( const QString & id )" );
    Item res( db_load( id ) );
    // Check if available
    if ( res.m_state != DB::Item::State::in ) {
        throw Exception( Errors::ItemUnavailable )
                << ( log.stream( warn )
                     << tr( "The item is not currently availably, it has state :")
                     << tr( res.m_state.toLatin1().constData() ) );
    }

    // Mark as booked
    QSqlQuery query;
    query_check_prepare( query, "update items set state=:state where id = :id" );
    query.bindValue( ":state", DB::Item::State::booked );
    query.bindValue( ":id", id );
    query_check_exec( query );
    res.m_state = DB::Item::State::booked;
    log.stream( info ) << "Booked item '" << id << "'";
    res.addEventLine( DB::Item::Event::booked,
                     QString( "Booked to contract '%1'. (Item::locate_and_book_in_db)").arg( contract_id ) );
    // Done, returning
    return res;

}

QString Item::getId() const {
    Logger log("QString Item::getId()");
    return m_id;
}

QString Item::getType() const {
    Logger log("QString Item::getType()");
    return m_type;
}


QString Item::getRentalGroup() const {
    Logger log("QString Item::getRentalGroup()");
    return m_rentalgroup;
}

QString Item::getState() const {
    Logger log("QString Item::getState() const");
    return m_state;
}

QString Item::toRentalHtml() const {
    Logger log( "QString Item::toRentalHtml() const" );
    QString res = QString( "<em>%1: %2/%3</em><br/>&nbsp;&nbsp;%4" )
                  .arg( m_id ).arg( m_type ).arg( m_rentalgroup );
    QString in = "";
    QString pre = "";
    if ( m_mark != "" ) {
        in += pre + m_mark;
        pre = "/";
    }
    if ( m_model != "" ) {
        in += pre + m_model;
        pre = "/";
    }
    if ( m_year != "" ) {
        in += pre + m_year;
        pre = "/";
    }
    if ( m_size != "" ) {
        in += pre + m_size;
        pre = "/";
    }
    return res.arg( in );
}

QString Item::toHtml() const {
    Logger log( "QString Item::toHtml() const" );
    return QString( "<li>%0</li>" ).arg( toRentalHtml() );
}

QString Item::toString() const {
    Logger log("QString Item::toString()");
    return QString( "id: '%0', type: '%1', size: '%2', mark: '%3', "
                    "model: '%4'', year: '%5', condition: '%6', price: '%7', rentalgroup: '%8', "
                    "state: '%9', note: '%10'" )
            .arg( m_id ).arg( m_type ).arg( m_size ).arg( m_mark ).arg( m_model )
            .arg( m_year ).arg( m_condition ).arg( m_price ).arg( m_rentalgroup )
            .arg( m_state ).arg( m_note );
}

QString Item::toReceiptString() const {
    Logger log("QString Item::toReceiptString() const");
    QString res = QString( "%0: %1/%2" )
                  .arg( m_id ).arg( m_type ).arg( m_size );
    if ( m_mark != "" ) {
        res += ", " + m_mark;
    }
    if ( m_model != "" ) {
        res += " " + m_model;
    }
    return res;
}

void Item::db_setToOut( const QString & reason ) {
    Logger log("void Item::db_setToIn()");
    if ( m_state != DB::Item::State::booked ) {
        throw Exception( Errors::ItemNotInBookedState )
                << ( log.stream(error)
                     << "Item with id: '" << m_id << "' not in '"
                     << DB::Item::State::booked << "' state, but in state '"
                     << m_state << "'");;
    }
    QSqlQuery query;
    query_check_prepare( query, "update items set state=:state where id=:id" );
    query.bindValue( ":id", m_id );
    query.bindValue( ":state", DB::Item::State::out );
    query_check_exec( query );
    addEventLine( DB::Item::Event::handed_out, reason + " (Item::db_setToOut)" );
    m_state = DB::Item::State::out;
}

void Item::db_setToIn( const QString & reason ) {
    Logger log("void Item::db_setToIn()");
    QSqlQuery query;
    query_check_prepare( query, "update items set state=:state where id=:id" );
    query.bindValue( ":id", m_id );
    query.bindValue( ":state", DB::Item::State::in );
    query_check_exec( query );
    addEventLine( DB::Item::Event::returned, reason + " (Item::db_setToIn)"  );
    m_state = DB::Item::State::in;
}

void Item::db_forceState( const QString &state, const QString & reason ) {
    Logger log("void Item::db_forceState(const QString &state)");
    if ( state != DB::Item::State::discarded
         && state != DB::Item::State::in
         && state != DB::Item::State::lost
         && state != DB::Item::State::maintenance ) {
        throw Exception( Errors::ItemForceStateOnIllegalState )
                << ( log.stream( error )
                     << "Illegal state '" << state << "' passed as argument to Item::forceState");
    }
    QSqlQuery query;
    query_check_prepare( query, "update items set state=:state where id=:id" );
    query.bindValue( ":id", m_id );
    query.bindValue( ":state",  state );
    query_check_exec( query );
    QString event = DB::Item::Event::other;
    if ( state == DB::Item::State::in ) {
        event = DB::Item::Event::returned;
    }
    if ( state == DB::Item::State::discarded ) {
        event = DB::Item::Event::discarded;
    }
    if ( state == DB::Item::State::lost ) {
        event = DB::Item::Event::lost;
    }
    if ( state == DB::Item::State::maintenance ) {
        event = DB::Item::Event::sent_for_maintenance;
    }

    addEventLine( event, reason + QString( " (State change to '%1' by Item::db_forceState)" )
                  .arg( state ) );
    m_state = state;
}

void Item::addEventLine(const QString &id, const QString &event, const QString &note) {
    Logger log("void Item::addEventLine(const QString &id, const QString &event, const QString &note)");
    QSqlQuery query;
    query_check_prepare( query, "insert into itemevents "
                         "(item_id, time, event, note) "
                         "values( :item_id, :time, :event, :note)");
    query.bindValue(":item_id", id );
    query.bindValue(":time", QDateTime::currentDateTime() );
    query.bindValue( ":event", event);
    query.bindValue( ":note", note );
    query_check_exec( query );
}

void Item::addEventLine(const QString &event, const QString &note) const {
    Logger log("void Item::addEventLine(const QString &event, const QString &note) const");
    addEventLine( m_id, event, note );
}


bool Item::db_exists(const QString &id) {
    Logger log("bool Item::db_exists(const QString &id)");
    QSqlQuery query;
    query_check_prepare( query, "select count(*) from items where id = :id" );
    query.bindValue( ":id", id );
    query_check_exec( query );
    query_check_first( query );
    return 0 != query.value( 0 ).toInt();
}

void Item::db_reid( const QString &from_id, const QString &to_id ) {
    Logger log( "void Item::db_reid( const QString &from_id, const QString &to_id )" );
    // This method would have been a lot easier, if "label" was not the same as "id"...

    log.stream( debug ) << "Will try to reid from id '" << from_id << "' to '" << to_id << "'";

    // Source must exist, dest must not
    if ( ! db_exists( from_id ) ) {
        throw Exception( Errors::ItemDoesNotExist )
                << ( log.stream( error )
                    << "Item with id '" << from_id << " does not exist, so it can not get a new id." );
    }
    if ( db_exists( to_id ) ) {
        throw Exception( Errors::ItemAlreadyExists )
                << ( log.stream( error )
                    << "Item with id '" << to_id << " already exist, so item with id '" << from_id << "' can not get this as a new id." );
    }

    // Make sure that the temp table is not in the way, so drop it now.
    {
        log.stream( debug ) << "Dropping temporary table temp_item_reid if it exists";
        QSqlQuery query;
        log.stream( debug ) << "Query.isActive is " << query.isActive()
                            << ", isValid is " << query.isValid();
        query_check_prepare( query,
                            "drop table if exists temp_item_reid" );
        query_check_exec( query );
    }
    // Now, copy all from from_id to to_id in items, using a temporary table.
    // The temporary table is needed because of foreign key constraints...
    // Need a transaction for this.
    database_transaction( "Item::db_reid( const QString &from_id, const QString &to_id )" );
    try {
        QSqlQuery query;
        log.stream( debug ) << "Creating temporary table to insert data that needs id change into";
        query_check_prepare( query, "create temporary table temp_item_reid as select * from items where id = :from_id");
        query.bindValue( ":from_id", from_id );
        query_check_exec( query );
        // Change the id
        log.stream( debug ) << "Changing the id of the item in the temporary table";
        query_check_prepare( query, "update temp_item_reid set id = :to_id" );
        query.bindValue( ":to_id", to_id );
        query_check_exec( query );
        // Copy back, that is, insert it into items
        log.stream( debug ) << "Copying modified item back into the items table";
        query_check_prepare( query,
                            "insert into items select * from temp_item_reid" );
        query_check_exec( query );
        log.stream( debug ) << "Modifying contractitems to point at the new id";
        // Now, update contract items and itemevents, to point at the new row
        query_check_prepare( query, "update contractitems set item_id = :to_id where item_id = :from_id");
        query.bindValue( ":to_id", to_id );
        query.bindValue( ":from_id", from_id );
        query_check_exec( query );
        log.stream( debug ) << "Modifying itemevents to point at the new id";
        query_check_prepare( query, "update itemevents set item_id = :to_id where item_id = :from_id");
        query.bindValue( ":to_id", to_id );
        query.bindValue( ":from_id", from_id );
        query_check_exec( query );
        // Delete the old item row
        log.stream( debug ) << "Deleting the old item";
        query_check_prepare( query, "delete from items where id = :from_id" );
        query.bindValue( ":from_id", from_id );
        query_check_exec( query );
        // Insert an event for the new id for this
        log.stream( todo ) << "Should add an event line at this point";
        addEventLine( to_id, DB::Item::Event::reid,
                     QString( "Reid from id '%1' to id '%2' (Item::db_reid)" )
                     .arg( from_id ).arg( to_id ) );
    }
    catch( ... ) {
        log.stream( error ) << "Got some kind of error during transaction, trying rollback";
        database_rollback( "Item::db_reid( const QString &from_id, const QString &to_id ) error catcher" );
        throw;
    }
    log.stream( debug ) << "All is well, about to commit";
    database_commit( "Item::db_reid( const QString &from_id, const QString &to_id ) - all well" );
    log.stream( info ) << "Has commit item id change from id '" << from_id << "' to '" << to_id << "'";

}
