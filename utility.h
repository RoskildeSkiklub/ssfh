#ifndef UTILITY_H
#define UTILITY_H

class QSqlQuery;

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



#endif // UTILITY_H
