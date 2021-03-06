/* * Snow Stuff For Hire - A POS-like system designed for very quick rental
 * operations at small ski resorts
 * Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
 *
 * This file is part of Snow Stuff For Hire.
 *
 * Snow Stuff For Hire is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Snow Stuff For Hire is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
 */

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
            static const QString reid;
            static const QString returned;
            static const QString sent_for_maintenance;
            static const QString status;
        };


    };
};

