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

#ifndef UTILITY_H
#define UTILITY_H

class QSqlQuery;
class QString;
class QStateMachine;
class QSqlDatabase;

#include <QStringList>

////////////////////////////////////////////////////////////
// DATABASE UTILITY FUNCTIONS

/** \brief Prepare a query, check that the result is OK
  * \param query The query to prepare
  * \param queryString The queryString to prepare
  *
  * If the query does not prepare OK, an error is logged, and an exception thrown. */
void query_check_prepare( QSqlQuery & query, const QString & queryString );


/** \brief Execute a query, check that the result is OK
  * \param query The query to execute
  *
  * If the query does not execute OK, an error is logged, and an exception thrown. */
void query_check_exec( QSqlQuery & query );

/** \brief Move to the first record in a query, check that the result is OK
  * \param query The query to move to the first record
  *
  * If the move is not OK, an error is logged, and an exception thrown. */
void query_check_first( QSqlQuery & query );

/** \brief Start a transaction
  * \param where The place in the code where we wished to start the transaction
  *
  * Starts a transaction, logs errors/ throws if unable to start */
void database_transaction( const QString & where );

/** \brief Commits a transaction
  * \param where The place in the code where we wished to stop the transaction
  *
  * Ends a transaction, logs errors/ throws if unable to end */
void database_commit( const QString & where );

/** \brief Rollbacks a transaction
  * \param where The place in the code where we wished to rollback the transaction
  *
  * Rollbacks a transaction, logs errors/ throws if unable to rollback */
void database_rollback( const QString & where );

/** \brief Retrieve a configuration value from the database
  * \param db The database connection, must be already open
  * \param key The key to get the value for */
QString database_get_config_value( const QSqlDatabase & db, const QString & key );

/** \brief Check version of sqlitedb, and enable foreign_keys
  * \param db The database connection, must be already open
  *
  * Checks the version of the db driver is at least the version stored in the configuration
  * in the database. Also, enables foreign keys. Throws on errors. */
void sqlite_check_setup( const QSqlDatabase & db );

/** \brief Check version of schema is equal to the one passed in
  * \param db The database connection, must be already open
  * \param version The exact version
  *
  * Lookups the db_version key in the configuration, and checks it against
  * the version passed in (equality required). Throws on errors */
void database_check_version( const QSqlDatabase & db, const QString & version );


//////////////////////////////////////////////////////////////
// VARIOUS STUFF

/** \brief Compare two version strings
  * \param vera Versionstring, on the form xx.yy.zz[.ff], all numbers
  * \param verb Versionstring, on the form xx.yy.zz[.ff], all numbers
  *
  * Compares vera with verb, and returns true if vera >= verb. The optional .ff part is ignored, if present */
  bool versionstring_qe( const QString & vera, const QString & verb );
/** \brief Show a todo dialog and log a message with level debug.
  * \param where Where the message is
  * \param msg The message of the todo */
void todofunc( const QString & where, const QString & msg );

/** \brief Macro to insert a todo call with function name */
#define TODO( m ) todofunc( __PRETTY_FUNCTION__, m );

/** \brief Get list of names of current states from a statemachine
  * \param stateMachine The statemachine instance to get the list from
  * \return QStringList with the names in
  *
  * This method extracts the object names from all states on the "configuration"
  * of the state machine and returns them in a list. It also logs them. */
QStringList get_current_states( const QStateMachine & stateMachine );

/** \brief Capitalize the first letter in each word
  * \param input The string to change
  * \return The string with each word capitalized first letter
  *
  * This changes e.g. MADS BONDO to Mads Bondo. */
QString capitalizeWords( const QString & input );



#endif // UTILITY_H
