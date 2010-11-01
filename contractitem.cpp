#include "contractitem.h"

// app
#include "log.h"
#include "db_consts.h"

using namespace Log;
ContractItem::ContractItem() : m_id( -1 ) {
    Logger log("ContractItem::ContractItem() : m_id( -1 )");
}

ContractItem::ContractItem(qlonglong contract_id, const QString &item_id,
                           const QString &rentalgroup, qlonglong price,
                           const QString &note )
                               : m_id( -1 ), m_contract_id( contract_id ),
                               m_item_id( item_id ),
                               m_state( DB::ContractItem::State::booked ),
                               m_rentalgroup( rentalgroup ),
                               m_price( price ), m_note( note ) {
    Logger log("ContractItem::ContractItem(qlonglong contract_id, const QString &item_id, const QString &rentalgroup, qlonglong price, const QString &note)");
    log.stream()
            << "m_id: '" << m_id
            << "', m_contract_id: '" << m_contract_id
            << "', m_item_id: '" << m_item_id
            << "', m_state: '" << m_state
            << "', m_rentalgroup '" << m_rentalgroup
            << "', m_price '" << m_price
            << "', m_note '" << m_note << "'";
}
