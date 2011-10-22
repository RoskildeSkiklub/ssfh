#ifndef EXCEPTION_H
#define EXCEPTION_H

// Qt
#include <QString>
#include <QTextStream>
#include <QCoreApplication>

// App
#include "log.h"

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

        DBVersionError = 1600, // Thrown if schema mismatch
        DBSqliteVersionError = 1601, // Thrown if sqlite driver mismatch

        ItemDoesNotExist = 2000,
        ItemAlreadyExists = 2001,
        ItemUnavailable = 2002,
        ItemNotInBookedState = 2003,
        ItemNotPartOfContract = 2004,
        ItemAlreadyPartOfContract = 2005,
        ItemForceStateOnIllegalState = 2006,

        ContractDoesNotExist = 3000,
        ContractNotInBookingState = 3001,
        ContractNotInActiveState = 3002,
        ContractNotInClosedState = 3003,
        ContractNotInParkedState = 3004,
        ContractStillHasReturnableItems = 3005,

        ContractItemDoesNotExist = 4001,

        PosPrinterNotOpen = 5000,
        PosImageTooLarge = 5001,
        PosImageUnableToConvert = 5002,
        PosPrinterIllegalFont = 5003,
        PosPrinterIllegalFontSize = 5004,

        HirerNotValid = 6000,

        DbMaintenanceForceStateOnIllegalState = 7000,

        MagswipeNoMatch = 10000,
        DKSundhedskortNoMatch = 10001,
        CreditCardNoMatch = 10002,
    };
};

class Exception {
    Q_DECLARE_TR_FUNCTIONS(Exception)
private:
    long m_statusCode;
    QString m_description;
    QString m_addInfo;
public:
    Exception( long statusCode, const QString & description, const QString & addInfo = "" )
        : m_statusCode( statusCode ), m_description( description ),
          m_addInfo( addInfo ) {};

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

    /** \brief Get the additional info
      * \return The additional info */
    QString getAddInfo() const {
        return m_addInfo;
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
