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
#include "posprinter.h"

using namespace Log;

////////////////////////////////////////////////////////////////////////////////
// Constructor

Contract::Contract()
    : m_id( -1 ),
    // TODO: Config + global config
    m_creationTime( QDateTime::currentDateTime() ),
    m_startTime( QDateTime::currentDateTime().addSecs( 600 ) ),
    m_endTime( m_startTime.addSecs( 3 * 3600 ) ),
    m_discount( 0 ),
    m_price( 0 ),
    m_payed( 0 ),
    m_state( "booking" ) {
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
    ContractItem cii;
    QList<ContractItem>::iterator i;
    for( i = m_contractItems.begin(); i != m_contractItems.end(); ++i) {
        if ( i->getItem().getId() == item_id ) {
            throw Exception( Errors::ItemAlreadyPartOfContract )
                    << ( log.stream( error ) << "Item '" << item_id
                         << "'' is already registered on the contract." );
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

    // TODO: Handle exceptions. Or transaction handle it.
    qlonglong itemPrice = calculateItemPrice( item );

    // Create a contract item, which is what is used as "lines" in the contract/display
    ContractItem ci( m_id, item, item.getRentalGroup(), itemPrice, "" );
    m_contractItems.append( ci );

    log.stream() << "Added item: " << item.toHtml();
    // TODO: Signal contract changed...
}

void Contract::returnItem(const QString &item_id) {
    Logger log("void Contract::returnItem(const QString &item_id)");
    checkInActiveState( "returnItem" );
    log.stream() << "Trying to return item with id '"
            << item_id << "', on contract with id '"
            << m_id << "'";
    // Check the item belongs to this contract
    QList<ContractItem>::iterator cii;
    bool foundit = false;
    for ( cii = m_contractItems.begin(); cii != m_contractItems.end(); ++cii ) {
        if ( cii->getItem().getId() == item_id ) {
            foundit = true;
            break;
        }
    }
    if ( ! foundit ) {
        throw Exception( Errors::ItemNotPartOfContract )
                << ( log.stream( error ) << "Item '" << item_id
                     << "'' is not registered on the contract." );

    }
    // Contract active, item part of it. Return it, or ignore if not out
    if ( cii->getState() != DB::ContractItem::State::out ) {
        log.stream( warn ) << "Expected state of ContractItem to be '"
                << DB::ContractItem::State::out << "', but it was '"
                << cii->getState() << "'";
    }

    if ( cii->getItem().getState() != DB::Item::State::out ) {
        log.stream( warn ) << "Expected state of Item to be '"
                << DB::Item::State::out << "', but it was '"
                << cii->getItem().getState() << "'";

    }
    // The approach is to update the Item first, then the ContractItem.
    // If anything fails, we reload the item.
    log.stream() << "Returning item with id '"
            << cii->getItem().getId() << "'";
    cii->getItem().db_setToIn();
    log.stream() << "Updating contractItem with id '"
            << cii->getId() << "'";
    cii->db_update_state( DB::ContractItem::State::returned );
    // update();

}

bool Contract::hasReturnableItems() const {
    Logger log("bool Contract::hasReturnableItems() const");
    QList<ContractItem>::const_iterator cii;
    for ( cii = m_contractItems.begin(); cii != m_contractItems.end(); ++cii ) {
        if ( cii->getItem().getState() == DB::Item::State::out ) {
            log.stream() << "Item with id '" << cii->getItem().getId()
                    << "' is returnable. Returning true.";
            return true;
        }
    }
    log.stream() << "No items found, returning false.";
    return false;
}

qlonglong Contract::calculateItemPrice(const Item &item) {
    Logger log("qlonglong Contract::calculateItemPrice(const Item &item)");
    log.stream( warn ) << "TODO: NOT IMPLEMENTED";
    return 42;
}

qlonglong Contract::getItemsPrice() const {
    Logger log("qlonglong Contract::getItemsPrice() const");
    qlonglong sum = 0;
    QList<ContractItem>::const_iterator i;
    for( i = m_contractItems.begin(); i != m_contractItems.end(); ++i) {
        sum += i->getPrice();
    }
    return sum;
}



////////////////////////////////////////////////////////////////////////////////
// CONTRACT

qlonglong Contract::getId() const {
    Logger log("qlonglong Contract::getId() const");
    return m_id;
}

QString Contract::getState() const {
    Logger log("QString Contract::getState() const");
    return m_state;
}

qlonglong Contract::getDiscount() const {
    Logger log("qlonglong Contract::getDiscount() const");
    return m_discount;
}

qlonglong Contract::getPayed() const {
    Logger log("qlonglong Contract::getPayed() const");
    return m_payed;
}

qlonglong Contract::getPayableAmount() const {
    Logger log("qlonglong Contract::getPayableAmount() const");
    return getItemsPrice() - getDiscount() - getPayed();
}

qlonglong Contract::getTotalPrice() const {
    Logger log("qlonglong Contract::getTotalPrice() const");
    return getItemsPrice() - getDiscount();
}

void Contract::activate() {
    Logger log("void Contract::activate()");
    checkInBookingState( "activate" );

    // TODO: Check payment.

    // We use a transaction for this.... right?
    database_transaction( "void Contract::activate()" );
    try {
        // First contract items and items
        QList<ContractItem>::iterator cii;
        for( cii = m_contractItems.begin(); cii != m_contractItems.end(); ++cii ) {
            cii->getItem().db_setToOut();
            cii->setState( DB::ContractItem::State::out );
            cii->db_insert();
        }
        // Then the contract - it just needs updating.
        m_state = DB::Contract::State::active;
        try {
            db_update();
        } catch ( ... ) {
            m_state = DB::Contract::State::booking;
            throw;
        }
        database_commit("void Contract::activate()");
    }
    catch( ... ) {
        database_rollback( "void Contract::activate()" );
        throw;
    }

}

void Contract::close() {
    Logger log("void Contract::close()");
    checkInActiveState("Contract::close()");
    if ( hasReturnableItems() ) {
        throw Exception( Errors::ContractStillHasReturnableItems )
                << ( log.stream( error )
                     << "Unable to close contract, because there are still returnable items." );
    }
    m_state = DB::Contract::State::closed;
    try {
        db_update();
    } catch ( ... ) {
        m_state = DB::Contract::State::active;
        throw;
    }
}


Contract Contract::db_load(qlonglong id) {
    Logger log("Contract Contract::db_load(qlonglong id)");
    log.stream() << "Loading contract with id '" << id << "'";
    //! \todo The error handling in this method should be much better.
    QSqlQuery query;
    log.stream() << "Loading contract data";
    query_check_prepare( query,
                         "select hirer, creationTime, startTime, endTime, "
                         "discount, price, payed, state, note "
                         "from contracts "
                         "where id=:id" );
    query.bindValue( ":id", id );
    query_check_exec( query );
    try {
        query_check_first( query );
    }
    catch (const Exception & e) {
        if ( e.getStatusCode() == Errors::DBResultError ) {
            throw Exception( Errors::ContractDoesNotExist )
            << ( log.stream( error )
                 //! \todo Translate???
                 << "The contract with id '" << id << "' does not exist" );
        }
        throw;
    }
    // Load values
    // First, Load the hirer
    log.stream() << "Loading hirer";
    Hirer hirer( query.value( 0 ).toLongLong( ) );
    log.stream() << "Creating contract";
    // Create and populate the contract
    Contract contract;
    contract.m_id = id;
    contract.m_hirer = hirer;
    contract.m_creationTime = query.value( 1 ).toDateTime();
    contract.m_startTime    = query.value( 2 ).toDateTime();
    contract.m_endTime      = query.value( 3 ).toDateTime();
    contract.m_discount     = query.value( 4 ).toLongLong();
    contract.m_price        = query.value( 5 ).toLongLong();
    contract.m_payed        = query.value( 6 ).toLongLong();
    contract.m_state        = query.value( 7 ).toString();
    contract.m_note         = query.value( 8 ).toString();
    // Now, get the contractItems
    log.stream() << "Loading ContractItems";
    query_check_prepare( query,
                         "select id from contractitems where contract_id=:contract_id");
    query.bindValue( ":contract_id", id );
    query_check_exec( query );
    while( query.next() ) {
        contract.m_contractItems.append( ContractItem::db_load( query.value( 0 ).toLongLong() ) );
    }
    // Done!
    log.stream() << "Contract loaded from database: " << contract.toString();
    return contract;
}

QString Contract::toString() const {
    Logger log("QString Contract::toString()");
    return QString( "id: '%0', hirer_id: '%1', creationTime: '%2', startTime: '%3', "
                    "endTime: '%4'', discount: '%5', price: '%6', payed: '%7', state: '%8', "
                    "note: '%9'" )
            .arg( m_id ).arg( m_hirer.m_id ).arg( m_creationTime.toString( Qt::ISODate) )
            .arg( m_startTime.toString( Qt::ISODate ) )
            .arg( m_endTime.toString( Qt::ISODate ) ).arg( m_discount ).arg( m_price ).arg( m_payed )
            .arg( m_state ).arg( m_note );
}

////////////////////////////////////////////////////////////
// HTML AND PRINT

QString Contract::toRentalHtml( ) const {
    Logger log("QString Contract::toRentalHtml() const");
    return toCommonHtml( true );
}

QString Contract::toReturnHtml( ) const {
    Logger log("QString Contract::toReturnHtml() const");
    return toCommonHtml( false );
}

QString Contract::toCommonHtml( bool isRental ) const {
    Logger log("QString Contract::toCommonHtml( bool isRental ) const");
    QString HEAD = "<head><style type=\"text/css\">"
                   ".total {font-weight:bold;font-size:x-large;}"
                   "</style>"
                   "</head>";

    QString res = HEAD + "<body><h3>" + tr( "Hirer information" ) + "</h3>";
    if ( ! m_hirer.isValid() ) {
        res += "<p><em>" + tr( "Please add a hirer by swiping an id card." ) + "</em></p>";
        return res;
    }
    res += QString( "<p>%0</p>" ).arg( m_hirer.toHtml() );

    // Duration
    res += "<h3>" + tr( "Duration of contract" ) + "</h3>"
           + "<p>" + tr( "Hand out: " ) + m_startTime.toString( Qt::ISODate ) + "<br>"
           + tr("Return no later than: " ) + m_endTime.toString( Qt::ISODate ) + "</p>";

    // Items.
    res += "<h3>" + tr( "Items in contract" ) + "</h3>";
    if ( m_contractItems.isEmpty() && isRental ) {
        res += "<p><em>" + tr( "Please add items by scanning barcodes on the items." ) + "</em></p>";
        return res;
    }
    res += "<table width=\"100%\">\n";
    if ( isRental ) {
        QList<ContractItem>::const_iterator i;
        for ( i = m_contractItems.begin(); i != m_contractItems.end(); ++i ) {
            res += i->toRentalHtml();
        }
    } else {
        QList<ContractItem>::const_iterator i;
        for ( i = m_contractItems.begin(); i != m_contractItems.end(); ++i ) {
            res += i->toReturnHtml();
        }
    }
    res += "</table>\n";

    // Total price
    res += "<h3>" + tr( "Totals" ) + "</h3>";
    res += "<table width=\"100%\">";
    res += QString( "<tr><td>%0</td><td align=\"right\">%1</td></tr>" )
           .arg( tr( "Total" ) ).arg( getItemsPrice() );
    if ( 0 != getDiscount() ) {
        res += QString( "<tr><td>%0</td><td align=\"right\">%1</td></tr>" )
               .arg( tr( "Discount" ) ).arg( getDiscount() );
    }
    if ( 0 != getPayed() ) {
        res += QString( "<tr><td>%0</td><td align=\"right\">%1</td></tr>" )
               .arg( tr( "Payed" ) ).arg( getPayed() );
    }
    res += QString( "<tr><td>%0</td><td align=\"right\" class=\"total\">%1</td></tr>" )
           .arg( tr( "Payable amount" ) ).arg( getPayableAmount() );
    res += "</table>";

    return res + "</body>";


};

void Contract::printRental(Pos::Printer &posPrinter) {
    Logger log("void Contract::printRental(Pos::Printer &posPrinter)");
    checkInActiveState( "void Contract::printRental" );

    // TODO: Would be better, if not hardcoded...
    // TODO: Should go into configuration, obviously.
    // posPrinter.startReceipt();

    // Logo and identifiers of the rental agency
    posPrinter << Pos::logo << Pos::endl
            << Pos::center << "Roskilde Skiklub" << Pos::endl
            << Pos::center << "Hedeland" << Pos::endl << Pos::endl;

    // Heading
    posPrinter << Pos::bold << Pos::center
            << tr( "Rental agreement" ) << Pos::endl << Pos::endl;

    // Various information about current time, and rental agreement duration
    // TODO: Need formats and translation for this.
    posPrinter << tr("Aftale id            : ") << Pos::endl;// << QString( m_id ) << Pos::endl;
    posPrinter << tr( "Dato                 : " )
            << QDateTime::currentDateTime().toString( Qt::ISODate )
            << Pos::endl;
    posPrinter << tr( "Udleveringstidspunkt : " )
            << m_startTime.toString( Qt::ISODate )
            << Pos::endl;
    posPrinter << tr( "Afleveringstidspunkt : " )
            << m_endTime.toString( Qt::ISODate )
            << Pos::endl;

    // Hirer.printRental( posPrinter );
    // TODO: Duration, hirer

    posPrinter << Pos::endl << tr( "Items :" );
    posPrinter << QString::fromUtf8( "æøå ÆØÅ\n" ) << Pos::endl; // TODO: Remove
    posPrinter << Pos::hr;
    ContractItem cii;
    QList<ContractItem>::const_iterator i;
    for ( i = m_contractItems.begin(); i != m_contractItems.end(); ++i ) {
        posPrinter << i->getItem().getId() << Pos::endl;
    }
    posPrinter << Pos::hr;

    // TODO: Sum and other stuff
}

//////////////////////////////////////////////////////////////
// MORE CONTRACT



void Contract::checkInBookingState( const QString & method ) {
    Logger log( "void Contract::checkInBookingState()" );
    if ( m_state != DB::Contract::State::booking ) {
        throw Exception( Errors::ContractNotInBookingState )
                << ( log.stream( error )
                     << QString( "Contract::%0 was called, but contract was not in expected state '%1', but in state '%2'" )
                     .arg( method ).arg( DB::Contract::State::booking ).arg( m_state ) );
    }
}

void Contract::checkInActiveState( const QString & method ) {
    Logger log( "void Contract::checkInActiveState()" );
    if ( m_state != DB::Contract::State::active ) {
        throw Exception( Errors::ContractNotInActiveState )
                << ( log.stream( error )
                     << QString( "Contract::%0 was called, but contract was not in expected state '%1', but in state '%2'" )
                     .arg( method ).arg( DB::Contract::State::active ).arg( m_state ) );
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
    query.bindValue( ":creationTime", m_creationTime );
    query.bindValue( ":state", m_state );
    query_check_exec( query );
    // Read back the last value
    query_check_prepare( query, "select last_insert_rowid() from contracts" );
    query_check_exec( query );
    query_check_first( query );
    m_id = query.value( 0 ).toLongLong();
    log.stream( info ) << "Created contract with id '" << m_id << "'";
}

void Contract::db_update() {
    Logger log("void Contract::db_update()");
    // Update price prior to database update.
    // This are always calculated when creating, but not when returning
    m_price = getTotalPrice();
    QSqlQuery query;
    query_check_prepare( query,
                         "update contracts "
                         "set hirer=:hirer, startTime=:startTime, "
                         "endTime=:endTime, discount=:discount, "
                         "price=:price, payed=:payed, "
                         "state=:state, note=:note "
                         "where id=:id" );
    query.bindValue( ":hirer", m_hirer.m_id );
    query.bindValue( ":startTime", m_startTime );
    query.bindValue( ":endTime", m_endTime );
    query.bindValue( ":discount", m_discount );
    query.bindValue( ":price", m_price );
    query.bindValue( ":payed", m_payed );
    query.bindValue( ":state", m_state );
    query.bindValue( ":note", m_note );
    query.bindValue( ":id", m_id );
    query_check_exec( query );
    log.stream( info ) << "Updated contract with id '" << m_id << "'";
}
