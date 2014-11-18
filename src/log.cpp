//
// Snow Stuff For Hire - A POS-like system designed for very quick rental
// operations at small ski resorts
// Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
//
// This file is part of Snow Stuff For Hire.
//
// Snow Stuff For Hire is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Snow Stuff For Hire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
//

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
