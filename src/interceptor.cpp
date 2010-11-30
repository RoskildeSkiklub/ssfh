#include "interceptor.h"

/** \file
  * \brief A class to intercept certain events, and pass the on to Detectors */

// Qt
#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QRegExp>
#include <QStringList>

// App
#include "log.h"
#include "dksundhedskort.h"
#include "exception.h"
#include "globals.h"

using namespace Log;

//TODO: Depending on how the magnetic card reader works, this can be refactored into
// some sort of generic matcher/translate to signal thingy. For now, YAGNI.

Interceptor::Interceptor()
    :  barcode_exp( "ABCD([\\dA-Z ]+)\r" ),
     magswipe_exp( DKSundhedskort::getRegExp() ),
//    magswipe_exp( QString::fromUtf8( "(?:%(.+)|\x03c\x086(.+)|%(.+)_\x03c\x086)(.+))_\\r") ),
    // magswipe_exp( QString::fromUtf8( "%(.+)_\x03c\x086)(.+)_\r") ),
    // magswipe_exp( QString::fromUtf8( "%(.+)_\x03c\x086)(.+)_\r") ),
    collecting( false ), timer( this ) {
    barcode_exp.setCaseSensitivity( Qt::CaseInsensitive ); // Needed, because user may press caps-lock....
    magswipe_exp.setCaseSensitivity( Qt::CaseInsensitive ); // Needed, because user may press caps-lock....

    timer.setInterval( 100 ); // Max 100 ms between "typing" stuff.
    timer.setSingleShot( true );
    connect( &timer, SIGNAL(timeout()), this, SLOT(timeout()) );
};

/** \brief Post the collected string, if no match
  *
  * This method is called, when the it is determined, that the currently collected string
  * will not match a scan event. It use the last_obj in order to post the string to the last
  * widget a keypress was received from, and sets up to ignore the the next event. (In order
  * not to restart on the same string).
  *
  * We split the post in two. Say the pattern we look for is ABCD. Now, if the user presses
  * A and we scan ABCD within 100 ms, the collecting will stop after AA, which will be posted, followed by
  * BCD, leading to widget input AABCD, instead of A and a scan event. Therefore, first A is posted, then the remainder of the
  * string. This ensures that we look at the remainder for matches too. This is probably overengineering it a bit... */

void Interceptor::postCollected() {
    Logger log( "Interceptor::postCollected()" );

    log.stream() << "Collected is '" << collected << "'";
    // Post the string to the widget that got the last keypress. Best we can do.
    if ( last_obj != 0 ) {
        next_text_ignore = collected.left(1);
        log.stream() << "Posting '" << collected.left( 1 ) << "'";
        QEvent * nev = new QKeyEvent ( QEvent::KeyPress, 0, Qt::NoModifier, collected.left( 1 ) );
        QApplication::postEvent( last_obj, nev );
        QString remainder = collected.right( collected.length() - 1 );
        if ( ! remainder.isEmpty() ) {
            log.stream() << "Posting remained of collected '" << remainder << "'";
            QEvent * nev = new QKeyEvent ( QEvent::KeyPress, 0, Qt::NoModifier, remainder );
            QApplication::postEvent( last_obj, nev );
        }
    } else {
        log.stream( warn ) << "last_obj was 0 - no posting taking place, input is probably lost. Collected was '" << collected << "'";
    }
    collected = "";
}

void Interceptor::timeout() {
    Logger log( "Interceptor::timeout()" );
    if ( collecting ) {
        log.stream() << "Giving up collecting because of timeout. Collected string is : '"
                << collected.toLocal8Bit().toPercentEncoding().constData() << "'";
        collecting = false;
        postCollected();
    }
}

bool Interceptor::eventFilter( QObject *obj, QEvent *ev ) {
    if ( ev->type() != QEvent::KeyPress ) {
        return false; // We only handle key press events
    }

    // The log is only constructed when certain it is key event - otherwise, we get too much noise in the log.
    Logger log( "bool Interceptor::eventFilter(QObject *obj, QEvent *ev)" );

    QKeyEvent * kev = dynamic_cast<QKeyEvent *>( ev );
    if ( !kev ) {
        log.stream( warn ) << "Got keypress event, but unable to dynamic_cast to QKeyEvent. Not good. Have to ignore it.";
        return false; // We only handle key events
    }
    log.stream() << "Intercepting key: '" << kev->key() << "', text is '" << kev->text().toLocal8Bit( ).toPercentEncoding( ).constData( ) << "'";
    if ( !next_text_ignore.isEmpty() && next_text_ignore == kev->text() ) {
        log.stream() << "Ignoring this keyevent, because of next_text_ignore match";
        return false;
    } else {
        next_text_ignore = "";
    }

    last_obj = obj; // Set up for possibly post of collected.
    // If not collecting, see if we can, if collecting, see if we can stop.
    if ( !collecting ) {
        // If we can get an exact match, signal event, but don't collect. (Rare, silly, border condition)
        if ( barcode_exp.exactMatch( kev->text() ) ) {
            log.stream( info ) << "NEW BARCODE EVENT (1): '"
                    << kev->text().toLocal8Bit().toPercentEncoding().constData() << "'";
            timer.stop();
            collected = "";
            emitBarcodeScan();
            return true;
        }
        // If we can get an exact match, signal event, but don't collect. (Rare, silly, border condition)
        if ( magswipe_exp.exactMatch( kev->text() ) ) {
            log.stream( info ) << "NEW MAGSWIPE EVENT (1): '"
                    << kev->text().toLocal8Bit().toPercentEncoding().constData() << "'";
            timer.stop();
            collected = "";
            emitMagSwipe();
            return true;
        }
        // No exact match. However, if we can have partial match, start collecting
        if ( barcode_exp.matchedLength() == kev->text().length() && kev->text().length() > 0 ) {
            log.stream() << "Starting collecting, because of partial match on barcode";
            collecting = true;
            collected = kev->text();
            timer.start();
            return true;
        }
        if ( magswipe_exp.matchedLength() == kev->text().length() && kev->text().length() > 0 ) {
            log.stream() << "Starting collecting, because of partial match on magswipe";
            collecting = true;
            collected = kev->text();
            timer.start();
            return true;
        }
        // No matches, don't start collecting, don't filter.
        return false;
    } else { // Collecting - see if we can get a complete match
        collected += kev->text();
        if ( barcode_exp.exactMatch( collected ) ) {
            log.stream( info ) << "NEW BARCODE EVENT (2): '"
                    << collected.toLocal8Bit().toPercentEncoding().constData() << "'";
            timer.stop();
            collected = "";
            collecting = false;
            emitBarcodeScan();
            return true;
        }
        if ( magswipe_exp.exactMatch( collected ) ) {
            log.stream( info ) << "NEW MAGSWIPE EVENT (2): '"
                    << collected.toLocal8Bit().toPercentEncoding().constData() << "'";
            timer.stop();
            collected = "";
            collecting = false;
            emitMagSwipe();
            return true;
        }
        // No exact match. We must have partial match on the collected string, to continue
        if ( barcode_exp.matchedLength() == collected.length() ) {
            log.stream() << "Continuing collecting, because of partial match on barcode";
            timer.start();
            return true;
        }
        // No exact match. We must have partial match on the collected string, to continue
        if ( magswipe_exp.matchedLength() == collected.length() ) {
            log.stream() << "Continuing collecting, because of partial match on magswipe";
            timer.start();
            return true;
        }
        // No exact, no partial match, just give it up, alright?
        collecting = false;
        log.stream() << "Giving up collecting. Collected string is : '"
                << collected.toLocal8Bit().toPercentEncoding().constData() << "'";
        postCollected();
        return true;
    }
};


void Interceptor::emitBarcodeScan() const {
    Logger log( "void Interceptor::emitBarcodeScan() const" );
    // Must match - get the cap value, convert to qulonglong
    if ( 1 != barcode_exp.captureCount() ) {
        log.stream( error ) << "Wrong number of captures in barcode_exp: "
                << barcode_exp.captureCount() << ". Not emitting barcodeScan";
        return;
    }
    QString code = barcode_exp.cap( 1 );
    // Check if this is a command or an id
    if ( code.startsWith( "C" ) ) {
        log.stream() << "Thinking '" << code << "' is a command scan";
        QRegExp cmdexp( "C(\\d+) {0,1}([A-Z]*)");
        if ( ! cmdexp.exactMatch( code ) ) {
            log.stream( error ) << "No exact command match on '" << code << "'. Not emitting barcodeScan";
            return;
        }
        Globals::BarcodeCommands::Command cmd ( static_cast<Globals::BarcodeCommands::Code> ( cmdexp.cap( 1 ).toInt() ), "", "" );
        if ( cmdexp.captureCount() == 2 ) {
            cmd.m_param = cmdexp.cap( 2 );
        }
        log.stream() << "Emitting barcodeCommandScan( { m_code: '" << cmd.m_code << "', m_param: '" << cmd.m_param << "' } )";
        emit barcodeCommandScan( cmd );
    } else {
        log.stream() << "Emitting barcodeItemScan( '" << code << "' )";
        emit barcodeItemScan( code );
    }
}

void Interceptor::emitMagSwipe() const {
    Logger log( "void Interceptor::emitMagSwipe() const" );
    log.stream() << "EMITTING MAGSWIPE";
    PROTECT_BLOCK(
            emit magSwipe( DKSundhedskort( magswipe_exp ) );
    );
}












