#include "log.h"

// Qt
#include <QDateTime>
#include <QStringList>

// app
#include "exception.h"

using namespace std;

// Simple NON-THREADED logger class

namespace Log {
    QFile Log::s_file;
    QTextStream Log::s_textStream;
    int Log::indent = 0;

    QString levelToString( Level level ) {
        switch( level ) {
        case trace: return "trace";
        case debug: return "debug";
        case todo: return "todo ";
        case info: return "info ";
        case warn: return "warn ";
        case error: return "error";
        case fatal: return "fatal";
        default: return "UNKN!";
        }
    }

    QString buildIndent( int indent ) {
        QStringList res;
        while ( indent > 0 ) {
            res << "| ";
            --indent;
        }
        return res.join( "" );
    }

    QDateTime Log::begin( Level level ) {
        QDateTime now( QDateTime::currentDateTime() );
        getTextStream() << now.toString( Qt::ISODate ) << now.time().toString( ".zzz" )
                << " : " << levelToString( level )
                << " : " << buildIndent( indent );
        return now;
    }

    void Log::init( const QString & filename ) {
        s_file.close();
        s_file.setFileName( filename );
        if ( ! s_file.open( QIODevice::WriteOnly | QIODevice::Truncate ) ) {
            throw Exception( Errors::UnableToOpenFile, QString( "Unable to open logfile '%0'" ).arg( filename ) );
        }
        s_textStream.setDevice( &s_file );
        indent = 0;
    };

}; // namespace
