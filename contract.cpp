#include "contract.h"

// App
#include "log.h"
#include "exception.h"

using namespace Log;

Contract::Contract() : m_id( -1 ) {
    Logger log( "Contract::Contract()" );
}

void Contract::add_item( const QString &item_id ) {
    Logger log( "void Contract::add_item( const QString &item_id )" );
    log.stream() << "Trying to add item '" << item_id << "'' to contract";

    // If item is already in our list, don't bother.
    Item i;
    foreach (i, m_items) {
        if ( i.m_id == item_id ) {
            throw Exception( Errors::ItemAlreadyPartOfContract )
                    << ( log.stream( error ) << "Item " << item_id << " is already registered on the contract." );
        }
    }

    // Try and load an item.
    Item item = Item::locate_book_in_db( item_id );
    // Got the item, add it.
    m_items.append( item );
    // TODO: Create contract item line
    // TODO: Create log line in database

    log.stream() << "Added item: " << item.toHtml();
    // TODO: Signal contract changed...
}

QString Contract::toHtml() const {
    QString res = "<h1>Hirer information</h1>";
    if ( ! m_hirer.isValid() ) {
        res += "<p><em>Please add a hirer by swiping an id card.</em></p>";
        return res;
    }
    res += m_hirer.toHtml();

    // Items.
    res += "<h1>Items in contract</h1>";
    if ( m_items.isEmpty() ) {
        res += "<p><em>Please add items by scanning barcodes on the items.</em></p>";
        return res;
    }
    res += items_to_html();
    return res;


};


QString Contract::items_to_html() const {
    Item i;
    QString res;
    foreach (i, m_items) {
        res += i.toHtml();
    }
    return res;
}
