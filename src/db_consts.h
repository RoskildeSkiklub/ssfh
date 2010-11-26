/** \file
  * \brief DB Constants
  * 
  * DB constants and translations, autogenerated from schema.sql 
  * by ./create-db-consts.pl */

// Qt
#include <QString>

/** \brief Define constants in the database as constants in the program. */
struct DB {
    struct Contract {
        struct State {
            static const QString active;
            static const QString booking;
            static const QString closed;
            static const QString parked;
        };


    };
    struct ContractItem {
        struct State {
            static const QString booked;
            static const QString out;
            static const QString returned;
        };


    };
    struct Item {
        struct State {
            static const QString booked;
            static const QString discarded;
            static const QString in;
            static const QString lost;
            static const QString maintenance;
            static const QString out;
        };

        struct Event {
            static const QString booked;
            static const QString created;
            static const QString deleted;
            static const QString discarded;
            static const QString handed_out;
            static const QString lost;
            static const QString other;
            static const QString returned;
            static const QString sent_for_maintenance;
        };


    };
};
