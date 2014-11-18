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

#ifndef LOG_H
#define LOG_H

// SINGLE-THREADED SIMPLE LOG IMPLEMENTATION.

// qt
#include <QTextStream>
#include <QString>
#include <QFile>
#include <QDateTime>



namespace Log {

    enum Level {
        trace,
        debug,
        todo,
        info,
        warn,
        error,
        fatal
    };

    // Static logger clas. Do not really use outside this module

    class Log {
        //! \brief Static stream for outputting stuff
        static QFile s_file;
        static QTextStream s_textStream;
        static int indent;
    public:
        Log() {};
        static void init( const QString & filename );
        static void shutdown() {
            s_textStream.flush();
            s_textStream.setDevice( NULL );
            s_file.close();
        }

        static QTextStream & getTextStream() {
            return s_textStream;
        }
        // Returns the current datetime
        static QDateTime begin( Level level );
        static void end() {
            getTextStream() << "\n" << flush;

        }
        static void in() {
            indent += 1;
        }
        static void out() {
            indent -= 1;
        }
    }; // Static log class.

    // Stream logger
    class LoggerStream {
    private:
        Level m_level;
        QString m_buf;
        QTextStream m_textStream;
    public:
        LoggerStream( Level level ) : m_level( level ){
            Log::begin( level );
            m_textStream.setString( &m_buf );
        }
        LoggerStream( const LoggerStream & src )
            : m_level( src.m_level ), m_buf( src.m_buf ), m_textStream( &m_buf ) {}

        template<class T> LoggerStream & operator <<( T val ) {
            // Log::getTextStream() << val;
            m_textStream << val;
            return *this;
        };
        ~LoggerStream() {
            Log::getTextStream() << m_buf;
            Log::end();
        }
        QString getBuffer() const {
            return m_buf;
        }
    };


    //! \brief Raii log thingy, for entering, exiting
    class Logger {
    private:
        QString m_location;
        QDateTime m_creationTime;
    public:
        Logger( const QString & location ) : m_location( location ) {
            m_creationTime = Log::begin( trace );
            Log::getTextStream() << ", Entering: " << m_location;
            Log::end();
            Log::in();
        }

        ~Logger() {
            Log::out();
            QDateTime endTime( Log::begin( trace ) );
            // Calculate difference
            qlonglong dms = m_creationTime.secsTo( endTime ) * 1000;
            dms += endTime.time().msec() - m_creationTime.time().msec();
            Log::getTextStream() << "` Leaving: " << m_location << ". ms: " << dms;
            Log::end();
        }

        LoggerStream stream( Level level = debug ) {
            return LoggerStream( level );
        }

    };
}; // namespace

#endif // LOG_H
