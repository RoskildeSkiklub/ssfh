#ifndef UTILITY_H
#define UTILITY_H

class QSqlQuery;
class QString;
class QStateMachine;

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

//////////////////////////////////////////////////////////////
// VARIOUS STUFF

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
