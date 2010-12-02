#include "contract.h"

// Qt
#include <QSqlQuery>
#include <QVariant>
#include <QMap>

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

void Contract::returnAll() {
    Logger log("bool Contract::returnAll()");
    QList<ContractItem>::const_iterator cii;
    // TODO: I am not entirely sure this is stable..
    for ( cii = m_contractItems.begin(); cii != m_contractItems.end(); ++cii ) {
        if ( cii->getItem().getState() == DB::Item::State::out ) {
            log.stream() << "Item with id '" << cii->getItem().getId()
                    << "' is returnable. Trying to return it";
            returnItem( cii->getItem().getId() );
        }
    }
    log.stream() << "All done";
}

bool Contract::hasReturnableItems() const {
    Logger log("bool Contract::hasReturnableItems() const");
    QList<ContractItem>::const_iterator cii;
    // Note, only the state of the contractitem can be checked, because of swaps, overrides, etc...
    for ( cii = m_contractItems.begin(); cii != m_contractItems.end(); ++cii ) {
        if ( cii->getState() == DB::ContractItem::State::out ) {
            log.stream() << "Item with id '" << cii->getItem().getId()
                    << "' is marked returnable in ContractItem '"
                    << cii->getId() << "'. Returning true.";
            return true;
        }
    }
    log.stream() << "No items found, returning false.";
    return false;
}

bool Contract::hasItems() const {
    Logger log("bool Contract::hasItems() const");
    return !m_contractItems.isEmpty();
}


qlonglong Contract::calculateItemPrice(const Item &item) {
    Logger log("qlonglong Contract::calculateItemPrice(const Item &item)");
    log.stream( warn ) << "TODO: NOT IMPLEMENTED";
    return 0;
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

QString Contract::getNote() const {
    Logger log("QString Contract::getNote() const");
    return m_note;
}

void Contract::setNote(const QString &note) {
    Logger log("void Contract::setNote(const QString &note)");
    m_note = note;
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
void Contract::park() {
    Logger log("void Contract::park()");
    checkInBookingState( "park" );

    // We use a transaction for this.... right?
    database_transaction( "void Contract::park()" );
    try {
        // First contract items and items
        QList<ContractItem>::iterator cii;
        for( cii = m_contractItems.begin(); cii != m_contractItems.end(); ++cii ) {
            // cii->getItem().db_setToOut(); Not done for park....
            // cii->setState( DB::ContractItem::State::out ); They are already booking
            cii->db_insert();
        }
        // Then the contract - it just needs updating.
        m_state = DB::Contract::State::parked;
        try {
            db_update();
        } catch ( ... ) {
            m_state = DB::Contract::State::booking;
            throw;
        }
        database_commit("void Contract::park()");
    }
    catch( ... ) {
        database_rollback( "void Contract::park()" );
        throw;
    }

}

void Contract::db_unPark() {
    Logger log("void Contract::db_unPark()");
    checkInParkedState( "db_unPark" );
    // We use a transaction for this.... right?
    database_transaction( "void Contract::db_unPark()" );
    QSqlQuery query;
    try {
        // First contract items and items
        QList<ContractItem>::iterator cii;
        for( cii = m_contractItems.begin(); cii != m_contractItems.end(); ++cii ) {
            query_check_prepare( query, "delete from contractitems where id=:id");
            query.bindValue(":id", cii->getId() );
            query_check_exec( query );
            cii->setId( -1 );
            cii->setState( DB::ContractItem::State::booked );
        }
        // Then the contract - it just needs updating.
        m_state = DB::Contract::State::booking;
        try {
            db_update();
        } catch ( ... ) {
            m_state = DB::Contract::State::parked;
            throw;
        }
        database_commit("void Contract::db_unPark()");
    }
    catch( ... ) {
        database_rollback( "void Contract::db_unPark()" );
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

void Contract::cancel() {
    Logger log("void Contract::cancel()");
    checkInBookingState( "Contract::cancel()");

    if ( m_id != -1 ) {
        QSqlQuery query; // For deleting stuff.

        log.stream( info ) << "Contract with id: '" << m_id << "' has been created in the database. Need to delete it. Checking ContractItems";
        QList<ContractItem>::iterator cii;
        for ( cii = m_contractItems.begin(); cii != m_contractItems.end(); ++cii ) {
            log.stream() << "Cancelling item with id '" << cii->getItem().getId() << "'";
            if ( cii->getItem().getState() != DB::Item::State::booked ) {
                log.stream( warn ) << "Expected state of Item to be '"
                        << DB::Item::State::booked << "', but it was '"
                        << cii->getItem().getState() << "'";
            }
            cii->getItem().db_setToIn();
            // If the contractItem has been inserted into the database, delete it
            log.stream() << "Deleting ContractItem with id '" << cii->getId() << "'";
            if ( cii->getId() != -1 ) {
                log.stream() << "ContractItem in database, deleting it";
                query_check_prepare( query, "delete from contractitems "
                                     " where id = :ci_id" );
                query.bindValue( ":ci_id", cii->getId() );
                query_check_exec( query );
            } else {
                log.stream() << "ContractItem not in database, ignoring it";
            }
        }
        // Finally, delete the contract
        query_check_prepare( query, "delete from contracts "
                             "where id=:id" );
        query.bindValue( ":id", m_id );
        query_check_exec( query );
        // Assigning to this is never pretty...
        *this = Contract();
        log.stream( info ) << "Contract has been cancelled";
    } else {
        log.stream() << "Contract has not been created in the database";
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
    QMap<QString,int> count_types;
    QList<ContractItem>::const_iterator i;
    for ( i = m_contractItems.begin(); i != m_contractItems.end(); ++i ) {
        if ( isRental ) {
            res += i->toRentalHtml();
        }  else {
            res += i->toReturnHtml();
        }
        if ( ! count_types.contains( i->getItem().getType() ) ) {
            count_types.insert( i->getItem().getType(), 1 );
        } else {
            count_types[i->getItem().getType()] += 1;
        }
    }
    res += "</table>\n";

    // List total count'n stuff
    res += "<h4>" + tr( "Count of types in contract") + "</h4>";
    QMap<QString,int>::const_iterator mi;
    for ( mi = count_types.begin(); mi != count_types.end(); ++mi ) {
        res += QString( "%0: %1<br/>" ).arg( mi.key() ).arg( mi.value() );
    }

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

    // Note
    if ( !m_note.isEmpty() ) {
        res += "<h3>" + tr( "Note" ) + "</h3>";
        res += "<p>" + m_note + "</p>";
    }

    return res + "</body>";


};

void Contract::printRental(Pos::Printer &posPrinter) {
    Logger log("void Contract::printRental(Pos::Printer &posPrinter)");
    checkInActiveState( "void Contract::printRental" );
    printGeneralReceipt( posPrinter, RentalAgreement );
 }

void Contract::printReceipt(Pos::Printer &posPrinter) {
    Logger log("void Contract::printReceipt(Pos::Printer &posPrinter)");
    checkInActiveState( "void Contract::printReceipt" );
    printGeneralReceipt( posPrinter, RentalReceipt );
}

void Contract::printReturn(Pos::Printer &posPrinter) {
    Logger log("void Contract::printReturn(Pos::Printer &posPrinter)");
    checkInClosedState( "void Contract::printReturn" );
    printGeneralReceipt( posPrinter, ReturnReceipt );
}


void Contract::printGeneralReceipt( Pos::Printer &posPrinter, ReceiptType receiptType ) {
    Logger log("printGeneralReceipt( Pos::Printer &posPrinter, ReceiptType receiptType )" );
    // TODO: Would be better, if the text and stuff was not hardcoded...
    // TODO: Name of rental agency, really should go into configuration, obviously.

    ///////////////////////////////////////////////////////////////////
    // Logo of the rental agency are only printed on stuff
    // that the customer gets to keep
    if ( receiptType != RentalAgreement ) {
        posPrinter << Pos::logo << Pos::endl << Pos::endl;
    };

    ///////////////////////////////////////////////////////////////////
    // Heading
    posPrinter.setFontSize( 1, 2 );
    posPrinter << Pos::bold << Pos::center;
    switch( receiptType ) {
    case RentalAgreement: posPrinter << tr("Rental Agreement"); break;
    case RentalReceipt: posPrinter << tr("Rental Receipt"); break;
    case ReturnReceipt: posPrinter << tr("Return Receipt"); break;
    }
    posPrinter << Pos::endl;
    posPrinter.setFontSize();

    ///////////////////////////////////////////////////////////////////
    // For the rental agreement, add the name of the club here
    if ( receiptType == RentalAgreement ) {
        posPrinter << Pos::center << "Roskilde Skiklub Hedeland" << Pos::endl;
    }
    posPrinter << Pos::endl;

    ////////////////////////////////////////////////////////////////////
    // Information about the contract comes next.
    // Figure out how to line it up
    int maxSize = qMax( tr( "Contract id" ).size(),
                        qMax( tr( "Time" ).size(),
                              qMax( tr( "Rental time").size(),
                                    tr( "Due back" ).size() ) ) );
    maxSize = 0 - maxSize;
    posPrinter << Pos::bold << QString( "%0 : " ).arg( tr( "Contract id"), maxSize )
            << QString( "%0" ).arg( m_id ) << Pos::endl;
    posPrinter << QString( "%0 : " ).arg( tr( "Time"), maxSize )
            << QDateTime::currentDateTime() << Pos::endl;
    posPrinter << QString( "%0 : " ).arg( tr( "Rental time" ), maxSize )
            << m_startTime << Pos::endl;
    // TODO: Eventually this will come back.
    /* posPrinter << QString( "%0 : " ).arg( tr( "Due back" ), maxSize )
            << m_endTime << Pos::endl; */
    posPrinter << QString( "%0 : " ).arg( tr( "Due back" ), maxSize )
            << m_endTime.date() << Pos::endl;
    posPrinter << Pos::endl;

    ////////////////////////////////////////////////////////////////////
    // Hirer - the id is not present on the stuff the user gets
    if ( receiptType == RentalAgreement) {
        posPrinter << Pos::bold << tr("Hirer id : ")
                << QString("%0").arg( m_hirer.m_id ) << Pos::endl;
    } else {
        posPrinter << Pos::bold << tr("Hirer:") << Pos::endl;
    }
    posPrinter << m_hirer.m_firstName << " " << m_hirer.m_lastName << Pos::endl;
    posPrinter << m_hirer.m_streetAddress << Pos::endl;
    posPrinter << m_hirer.m_zip << "  " << m_hirer.m_city << Pos::endl;
    if ( !m_hirer.m_note.isEmpty() ) {
        posPrinter << m_hirer.m_note << Pos::endl;
    }
    posPrinter << Pos::endl;

    ////////////////////////////////////////////////////////////////////
    // Items.
    // On agreement and rental receipt, cost is listed, on return, state of item
    posPrinter << Pos::bold << tr( "Items:" ) << Pos::hr;
    QMap<QString,int> count_types;
    QList<ContractItem>::const_iterator i;
    for ( i = m_contractItems.begin(); i != m_contractItems.end(); ++i ) {
        const ContractItem & cii = *i;
        QString idesc = QString( "%0/%1")
                        .arg( cii.getItem().getType() )
                        .arg( cii.getRentalgroup() );
        if ( receiptType == ReturnReceipt ) {
            // This prints the state of the item
            posPrinter << idesc << Pos::bold
                    << QString( "%0")
                    .arg( ContractItem::tr( cii.getState().toLatin1().constData() ),
                          posPrinter.getReceiptWidth() - idesc.size() ) << Pos::endl;
        } else {
            // This prints the cost of the item
            // TODO: When sale is added, these must be marked in a special way.
            posPrinter << idesc
                    << QString( "%0,00")
                    .arg( cii.getPrice(),
                          posPrinter.getReceiptWidth() - 3 - idesc.size() ) << Pos::endl;
        }
        posPrinter << "   " << cii.getItem().toReceiptString() << Pos::endl;
        // Track total count
        if ( ! count_types.contains( i->getItem().getType() ) ) {
            count_types.insert( i->getItem().getType(), 1 );
        } else {
            count_types[i->getItem().getType()] += 1;
        }
    }
    posPrinter << Pos::hr;
    // Dump a total count
    QMap<QString,int>::const_iterator mi;
    for ( mi = count_types.begin(); mi != count_types.end(); ++mi ) {
        posPrinter << tr( "Count " )
                << QString( "%0: %1" ).arg( mi.key() ).arg( mi.value() )
                << Pos::endl;
    }
    posPrinter.setFontSize( 1, 2 );
    // The sum of the items is printed, except for returns, where it is the state of the contract
    // TODO: Discount and stuff is missing from this.
    if ( receiptType == ReturnReceipt ) {
        posPrinter << Pos::center;
        if ( hasReturnableItems() ) {
            posPrinter << tr( "Some items not returned" );
        } else {
            posPrinter << tr( "All items returned" );
        }
    } else {
        posPrinter << tr("Total") << QString( "%0,00" )
                .arg( getTotalPrice(),
                      posPrinter.getReceiptWidth() - 3 - tr("Total").size() )
                << Pos::endl;
    }
    posPrinter.setFontSize();
    posPrinter << Pos::endl << Pos::endl;

    ///////////////////////////////////////////////////////////////////////////////
    // Finally, some blurbs
    // Thank you/Conditions
    if ( receiptType == ReturnReceipt ) {
        posPrinter << Pos::center << tr( "Thank you for visiting");
    } else {
        posPrinter << Pos::center << tr("Conditions available upon request");
    }
    posPrinter << Pos::endl << Pos::endl;

    // Signature or ident
    if ( receiptType == RentalAgreement ) {
        posPrinter << Pos::endl << Pos::endl;
        posPrinter.setFont( Pos::FontB );
        posPrinter << Pos::bold
                << tr( "Signature : " )
                << QString( "%0" )
                .arg( "", posPrinter.getReceiptWidth() - tr( "Signature : ").size(), '_' ) << Pos::endl;
        posPrinter.setFont( Pos::FontA );
        posPrinter << Pos::endl;
    } else {
        posPrinter << Pos::center << "Roskilde Skiklub Hedeland";
        if ( receiptType == RentalReceipt ) {
            posPrinter << Pos::center << "CVR: 33510055";
        }
        posPrinter << Pos::center << "www.roskildeskiklub.dk" << Pos::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    // Add some space to allow easy handling.
    posPrinter << Pos::endl << Pos::endl;
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

void Contract::checkInParkedState( const QString & method ) {
    Logger log( "void Contract::checkInParkedState()" );
    if ( m_state != DB::Contract::State::parked ) {
        throw Exception( Errors::ContractNotInParkedState )
                << ( log.stream( error )
                     << QString( "Contract::%0 was called, but contract was not in expected state '%1', but in state '%2'" )
                     .arg( method ).arg( DB::Contract::State::parked ).arg( m_state ) );
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

void Contract::checkInClosedState( const QString & method ) {
    Logger log( "void Contract::checkInClosedState()" );
    if ( m_state != DB::Contract::State::closed) {
        throw Exception( Errors::ContractNotInClosedState )
                << ( log.stream( error )
                     << QString( "Contract::%0 was called, but contract was not in expected state '%1', but in state '%2'" )
                     .arg( method ).arg( DB::Contract::State::closed ).arg( m_state ) );
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
