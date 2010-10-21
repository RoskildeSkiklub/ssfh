// C++
#include <iostream>

// Qt
#include <QtGui/QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QEvent>
#include <QStringList>
#include <QSqlQuery>

// app
#include "globals.h"
#include "interceptor.h"
#include "mainwindow.h"
#include "log.h"
#include "exception.h"
#include "utility.h"

/** \brief Toplevel error handling, handled through QApplication override */
class RshApplication : public QApplication {
public:
    RshApplication(int & argc, char **argv) : QApplication( argc, argv ) {
        // Log::Logger log( "Constructed RshApplication" );
    }
    virtual ~RshApplication() {}
    virtual bool notify( QObject * receiver, QEvent * event ) {
        // return QApplication::notify( receiver, event );
        try {
            return QApplication::notify( receiver, event );
        }
        catch( const Exception & e ) {
            Log::Logger log( "RshApplication::notify - Exception & e" );
            default_critical( log, e.toString() );
        }
        catch( const QString & e ){
            Log::Logger log( "RshApplication::notify - QString & e" );
            default_critical( log, e );
        }
        catch( const std::exception & e ) {
            Log::Logger log( "RshApplication::notify - std::exception & e" );
            default_critical( log, e.what() );
        }
        catch( const std::string & e ){
            Log::Logger log( "RshApplication::notify - std::string & e" );
            default_critical( log, e.c_str() );
        }
        catch( const char * e ){
            Log::Logger log( "RshApplication::notify - const char * e" );
            default_critical( log, e );
        }
        catch( ... ) {
            Log::Logger log( "RshApplication::notify - ..." );
            default_critical( log, Exception::tr( "Unknown error") );

        }

        return false; // We did not actually handle the event.
    }
};

int main(int argc, char **argv) {
    RshApplication app(argc, argv);
    int res = 0;

    // Need two arguments, the path to the database, and the path to the log file.
    QStringList args = app.arguments();
    if ( args.size() != 3 ) {
        std::cerr << "Usage: " << args.at(0).toLocal8Bit().constData() << " <databasefile> <logfile>" << std::endl;
        return -1;
    }

    QString databasefile = args.at(1);
    QString logfile = args.at(2);

    std::cout << QDateTime::currentDateTime().toString( Qt::ISODate ).toLocal8Bit().constData() << " : Starting SnowStuffForHire" << std::endl
            << "Database: '" << databasefile.toLocal8Bit().constData() << "'" << std::endl
            << "Log: '" << logfile.toLocal8Bit().constData() << "'" << std::endl;

    // Set up log
    try {
        Log::Log::init( logfile );
    } catch ( Exception & e ) {
        std::cerr << "Unable to start log system" << std::endl;
        QMessageBox::critical( NULL, "Unable to start log system", QString( "Unable to start log system. Message is '%0'" ).arg( e.toString() ) );
        return -1;
    }

    {
        Log::Logger log( "main" );

        // Translation stuff
        QTranslator qtTranslator;
        log.stream() << "Loading qtTranslator from " << "qt_" << QLocale::system().name()
                << ", location: " << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        if ( ! qtTranslator.load("qt_" + QLocale::system().name(),
                                 QLibraryInfo::location(QLibraryInfo::TranslationsPath)) ) {
            log.stream( Log::warn ) << "Failed to load global translation files for locale";
        }
        app.installTranslator(&qtTranslator);

        QTranslator myappTranslator;
        log.stream() << "Loading QTranslator from " << "SnowStuffForHire_" << QLocale::system().name();
        if ( ! myappTranslator.load("SnowStuffForHire_" + QLocale::system().name()) ) {
            log.stream( Log::warn ) << "Failed to load application translation files for locale";
        }
        app.installTranslator(&myappTranslator);

        // Set up the event handler for our application
        Interceptor interceptor;
        Globals::interceptor = &interceptor;
        app.installEventFilter( &interceptor );

        // Database stuff
        log.stream() << "Setting up database";
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        if ( ! db.isValid() ) {
            log.stream( Log::fatal ) << "Unable to load QSQLITE database driver";
            QMessageBox::critical( NULL, "Unable to load QSQLITE database driver", QString( "Unable to load QSQLITE database driver. Aborting." ) );
            return -1;
        }
        db.setHostName("localhost");
        db.setDatabaseName( databasefile );
        db.setUserName("");
        db.setPassword("");
        if ( ! db.open() ) {
            log.stream( Log::fatal ) << "Unable to open database '" << databasefile << "'";
            QMessageBox::critical( NULL, "Unable to open database", QString( "Unable to open database specified as '%0'. Aborting." ).arg( databasefile ) );
            return -1;
        }
        log.stream() << "Database successfully opened, performing test select";
        try {
            QSqlQuery query;
            query_check_prepare( query, "select * from itemtypes" );
            query_check_exec( query );
        }
        catch( ... ) {
            log.stream( Log::fatal ) << "Unable to perform initial select on database '" << databasefile << "'";
            QMessageBox::critical( NULL, "Unable to perform initial select on database", QString( "Unable to perform initial select on database '%0'. Aborting." ).arg( databasefile ) );
            return -1;
        }

        // Mainwindow stuff
        log.stream() << "Creating mainwindow";
        MainWindow w;
        log.stream() << "Showing mainwindow";
        w.show();
        log.stream() << "Executing application";
        res = app.exec();
        log.stream() << "Application done, shutting down";
    }
    Log::Log::shutdown();
    return res;
}
