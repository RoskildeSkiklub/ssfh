#include "log.h"

#include <QDateTime>

// app
#include "exception.h"

// Simple NON-THREADED logger class

namespace Log {
    QFile Log::s_file;
    QTextStream Log::s_textStream;
    int Log::indent = 0;

    QString levelToString( Level level ) {
        switch( level ) {
        case trace: return "trace";
        case debug: return "debug";
        case info: return "info ";
        case warn: return "warn ";
        case error: return "error";
        case fatal: return "fatal";
        default: return "UNKN!";
        }
    }

    void Log::begin( Level level ) {
        getTextStream() << QDateTime::currentDateTime().toString( Qt::ISODate )
                    << " : " << levelToString( level )
                    << " : " << QString( indent, ' ' ) << QString( indent, ' ' );
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
