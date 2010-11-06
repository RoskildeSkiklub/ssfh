#ifndef EXCEPTION_H
#define EXCEPTION_H

// Qt
#include <QString>
#include <QTextStream>
#include <QCoreApplication>

// App
#include <log.h>

namespace Errors {
    enum {
        UnableToOpenFile = 1,
        InternalError = 100,
        NotImplemented = 101,

        DBError = 1000,
        DBTransactionError = 1001,
        DBCommitError = 1002,
        DBRollbackError = 1003,
        DBResultError = 1004,
        DBConsistencyError = 1005,
        DBQueryInActive = 1500,

        ItemDoesNotExist = 2000,
        ItemUnavailable = 2001,
        ItemNotInBookedState = 2002,
        ItemNotPartOfContract = 2003,
        ItemAlreadyPartOfContract = 2004,

        ContractDoesNotExist = 3000,
        ContractNotInBookingState = 3001,
        ContractNotInActiveState = 3002,

        ContractItemDoesNotExist = 4001,

        MagswipeNoMatch = 10000,
    };
};

class Exception {
    Q_DECLARE_TR_FUNCTIONS(Exception)
private:
    long m_statusCode;
    QString m_description;
public:
    Exception( long statusCode, const QString & description )
        : m_statusCode( statusCode ), m_description( description ) {};

    Exception( long statusCode ) : m_statusCode( statusCode ) {};

/*    void out( QTextStream & stream ) const {
        stream << "Exception: " << m_statusCode << ": " << m_description;

    } */
    QString toString() const;

    Exception & operator <<( const Log::LoggerStream & logstream ) {
        m_description = logstream.getBuffer();
        return *this;
    }

    /** \brief Get the statuscode
      * \return The statuscode of the exception */
    long getStatusCode() const {
        return m_statusCode;
    };

};

// Show a default message box in case of exceptions
void default_critical( Log::Logger & log, const QString & msg );

// This is some stuff to make sure that we do not propagate exceptions to the main event handler...
// Note, that this assumes the existence of a local log object. Yes, this is sort of
// bad, but I am lazy.
#define PROTECT_BLOCK( block ) try { block; } \
  catch( const Exception & e ){ default_critical( log, e.toString() ); } \
  catch( const QString & e ){ default_critical( log, e ); } \
  catch( const std::exception & e ) { default_critical( log, e.what() ); }\
  catch( const std::string & e ){ default_critical( log, e.c_str() ); }\
  catch( const char * e ){ default_critical( log, e ); }\
  catch( ... ) { default_critical( log, Exception::tr( "Unknown error") ); };

#endif // EXCEPTION_H
