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
    addEventLine( DB::Item::Event::created, "" );
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

Item Item::locate_and_book_in_db( const QString & id ) {
    Logger log( "Item locate_book_in_db( const QString & id )" );
    Item res( db_load( id ) );
    // Check if available
    if ( res.m_state != DB::Item::State::in ) {
        throw Exception( Errors::ItemUnavailable )
                << ( log.stream( warn )
                     << tr( "The item is not currently availably, it has state :")
                     << tr( res.m_state.toAscii().constData() ) );
    }

    // Mark as booked
    QSqlQuery query;
    query_check_prepare( query, "update items set state=:state where id = :id" );
    query.bindValue( ":state", DB::Item::State::booked );
    query.bindValue( ":id", id );
    query_check_exec( query );
    res.m_state = DB::Item::State::booked;
    log.stream( info ) << "Booked item '" << id << "'";
    res.addEventLine( DB::Item::Event::booked, "" );
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

void Item::db_setToOut() {
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
    addEventLine( DB::Item::Event::handed_out, "" );
    m_state = DB::Item::State::out;
}

void Item::db_setToIn() {
    Logger log("void Item::db_setToIn()");
    QSqlQuery query;
    query_check_prepare( query, "update items set state=:state where id=:id" );
    query.bindValue( ":id", m_id );
    query.bindValue( ":state", DB::Item::State::in );
    query_check_exec( query );
    addEventLine( DB::Item::Event::returned, "" );
    m_state = DB::Item::State::in;
}

void Item::db_forceState(const QString &state) {
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

    addEventLine( event, QString( "State change to %0 by Item::db_forceState")
                  .arg( state ) );
    m_state = state;
}

void Item::addEventLine(const QString &event, const QString &note) const {
    Logger log("void Item::addEventLine(const QString &event, const QString &note) const");
    QSqlQuery query;
    query_check_prepare( query, "insert into itemevents "
                         "(item_id, time, event, note) "
                         "values( :item_id, :time, :event, :note)");
    query.bindValue(":item_id", m_id );
    query.bindValue(":time", QDateTime::currentDateTime() );
    query.bindValue( ":event", event);
    query.bindValue( ":note", note );
    query_check_exec( query );
}