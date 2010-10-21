// Qt
#include <QtGui/QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QEvent>

// app
#include "globals.h"
#include "interceptor.h"
#include "mainwindow.h"
#include "log.h"
#include "exception.h"

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
    int res;

    // Set up log
    Log::Log::init( "/tmp/ssfh.log");
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
        // TODO: This wont do in production...
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setHostName("localhost");
        db.setDatabaseName("/home/madsdyd/Kode/RSK-mockup/SnowStuffForHire/schema/test.db");
        db.setUserName("");
        db.setPassword("");
        // fix up.
        if ( ! db.open() ) {
            QMessageBox::critical( NULL, "Unable to open database", "Unable to open database, Mads, aborting" );
            exit(1);
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
