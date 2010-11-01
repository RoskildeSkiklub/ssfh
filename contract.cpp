#include "contract.h"

// Qt
#include <QSqlQuery>
#include <QVariant>

// App
#include "log.h"
#include "exception.h"
#include "db_consts.h"
#include "contractitem.h"
#include "utility.h"

using namespace Log;

////////////////////////////////////////////////////////////////////////////////
// Constructor

Contract::Contract() : m_id( -1 ), m_state( "booking" ) {
    Logger log( "Contract::Contract()" );
}

////////////////////////////////////////////////////////////////////////////////
// HIRER MANIPULATION
bool Contract::setHirer(const Hirer &hirer) {
    Logger log("void Contract::setHirer(const Hirer &hirer)");
    checkInBookingState( "setHirer" );

    if ( ! hirer.isValid() ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error ) << "Hirer passed to Contract::setHirer was invalid." );
    }
    if ( m_hirer.isValid() ) {
        log.stream( error ) << "Hirer with id '" << m_hirer.m_id << "' already set for contract with id '" << m_id << "'";
        return false;
    }

    log.stream() << " Setting hirer with id '" << hirer.m_id << "' for contract with id '" << m_id << "'";
    m_hirer = hirer;
    return true;
}

Hirer & Contract::getHirer() {
    Logger log("Hirer & Contract::getHirer() const");
    return m_hirer;
}

////////////////////////////////////////////////////////////////////////////////
// DURATION

void Contract::setDuration(const QDateTime &start, const QDateTime &end) {
    Logger log("void Contract::setDuration(const QDateTime &start, const QDateTime &end)");
    checkInBookingState( "setDuration" );
    throw "Contract::setDuration not implemented";
}

////////////////////////////////////////////////////////////////////////////////
// ITEM



void Contract::addItem( const QString &item_id ) {
    Logger log( "void Contract::add_item( const QString &item_id )" );
    log.stream() << "Trying to add item '" << item_id << "'' to contract";
    checkInBookingState( "addItem" );

    // If item is already in our list, don't bother.
    Item i;
    foreach (i, m_items) {
        if ( i.getId() == item_id ) {
            throw Exception( Errors::ItemAlreadyPartOfContract )
                    << ( log.stream( error ) << "Item " << item_id << " is already registered on the contract." );
        }
    }

    // If the contract has id -1, then create it in the database, and reread the number
    if ( m_id == -1 ) {
        log.stream( info ) << "Contract is not yet created in the database. Creating it";
        db_insert();
    }

    // Try and load an item.
    // TODO: Database transaction - I guess so...
    Item item = Item::locate_and_book_in_db( item_id );
    // Got the item, add it.
    m_items.append( item );

    // TODO: Handle exceptions. Or transaction handle it.
    qlonglong itemPrice = calculateItemPrice( item );

    ContractItem ci( m_id, item_id, item.getRentalGroup(), itemPrice, "" );
    // TODO: Create contract item line

    log.stream() << "Added item: " << item.toHtml();
    // TODO: Signal contract changed...
}

qlonglong Contract::calculateItemPrice(const Item &item) {
    Logger log("qlonglong Contract::calculateItemPrice(const Item &item)");
    log.stream( warn ) << "TODO: NOT IMPLEMENTED";
    return 42;
}


////////////////////////////////////////////////////////////////////////////////
// CONTRACT


//TODO Translate...
QString Contract::toHtml() const {
    QString res = "<h1>Hirer information</h1>";
    if ( ! m_hirer.isValid() ) {
        res += "<p><em>Please add a hirer by swiping an id card.</em></p>";
        return res;
    }
    res += m_hirer.toHtml();

    // Duration
    res += "<h1>Duration of contract</h1><p>TODO: Implement</p>";

    // Items.
    res += "<h1>Items in contract</h1>";
    if ( m_items.isEmpty() ) {
        res += "<p><em>Please add items by scanning barcodes on the items.</em></p>";
        return res;
    }
    res += items_to_html();
    return res;


};

void Contract::checkInBookingState( const QString & method ) {
    Logger log( "void Contract::checkInBookingState()" );
    if ( m_state != DB::Contract::State::booking ) {
        throw Exception( Errors::ContractNotInBookingState )
                << ( log.stream( error )
                     << QString( "Contract::%0 was called, but contract was not in expected state '%1', but in state '%2'" )
                     .arg( method ).arg( DB::Contract::State::booking ).arg( m_state ) );
    }
}


// Inserts a very rough contract, with almost no information at all.
void Contract::db_insert() {
    Logger log("void Contract::db_insert()");
    checkInBookingState("db_insert()");
    if ( !m_hirer.isValid() ) {
        throw Exception( Errors::ContractNotInBookingState )
                << ( log.stream( error )
                     << QString( "Contract::db_insert was called, but contract did not have a valid hirer" ) );
    }
    QSqlQuery query;
    query_check_prepare( query,
                         "insert into contracts ( hirer, creationTime, state ) "
                         "values( :hirer, :creationTime, :state )" );
    query.bindValue( ":hirer", m_hirer.m_id );
    query.bindValue( ":creationTime", QDateTime::currentDateTime() );
    query.bindValue( ":state", m_state );
    query_check_exec( query );
    // Read back the last value
    query_check_prepare( query, "select last_insert_rowid() from contracts" );
    query_check_exec( query );
    query_check_first( query );
    m_id = query.value( 0 ).toLongLong();
    log.stream( info ) << "Created contract with id '" << m_id << "'";
}

QString Contract::items_to_html() const {
    Item i;
    QString res;
    foreach (i, m_items) {
        res += i.toHtml();
    }
    return res;
}
