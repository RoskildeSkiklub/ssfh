#ifndef UTILITY_H
#define UTILITY_H

class QSqlQuery;
class QString;

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


/** \brief Show a todo dialog and log a message.
  * \param where Where the message is
  * \param msg The message of the todo */
void todo( const QString & where, const QString & msg );

/** \brief Macro to insert a todo call with function name */
#define TODO( m ) todo( __PRETTY_FUNCTION__, m );

#endif // UTILITY_H
