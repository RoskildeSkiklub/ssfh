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
#include "creditcard.h"
#include "exception.h"
#include "globals.h"


using namespace Log;

// The magsswipt markers are  - unfortunately - translated by the keyboard input system.
// In danish (utf8), that is:
// \x025 (%) = track 1
// \x0c3\x086 (danish ligatur ae) = track 2. This is suspectible to caps lock.
// \x060 (`) = track 3.

const QString mag_track1start = QString::fromUtf8( "\x025" );
const QString mag_track2start = QString::fromUtf8( "\x0c3\x086" );
const QString mag_track3start = QString::fromUtf8( "\x060" );
const QString mag_trackEnd = QString("_");
const QString mag_end = QString( "\r" );


//TODO: Depending on how the magnetic card reader works, this can be refactored into
// some sort of generic matcher/translate to signal thingy. For now, YAGNI.

Interceptor::Interceptor()
    :  barcode_exp( "ABCD([\\dA-Z ]+)\r" ),
    // The magsswipt markers are  - unfortunately - translated by the keyboard input system.
    // In danish (utf8), that is:
    // \x025 (%) = track 1
    // \x0c3\x086 (danish ligatur ae) = track 2. This is suspectible to caps lock.
    // \x060 (`) = track 3.
    magswipe_exp( QString( "(?:%0|%1|%2).+%3%4" )
                  .arg( mag_track1start ).arg( mag_track2start ).arg( mag_track3start )
                  .arg( mag_trackEnd ).arg( mag_end ) ),


            // QString::fromUtf8( "(?:\x025|\x0c3\x086|\x060).+_\r") ),
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

    // This emits one of three possibilities, either a command,
    // an SSN scan (10 digits) or an item scan (everything else).

    // Must match - get the cap value, convert to qulonglong
    if ( 1 != barcode_exp.captureCount() ) {
        log.stream( error ) << "Wrong number of captures in barcode_exp: "
                << barcode_exp.captureCount() << ". Not emitting barcodeScan";
        return;
    }
    QString code = barcode_exp.cap( 1 );
    // Check if this is a command or an id
    if ( code.toUpper().startsWith( "C" ) ) {
        log.stream() << "Thinking '" << code << "' is a command scan";
        QRegExp cmdexp( "C(\\d+) {0,1}([A-Z]*)");
        if ( ! cmdexp.exactMatch( code.toUpper() ) ) {
            log.stream( error ) << "No exact command match on '" << code << "'. Not emitting barcodeScan";
            return;
        }
        Globals::BarcodeCommands::Command cmd ( static_cast<Globals::BarcodeCommands::Code> ( cmdexp.cap( 1 ).toInt() ), "", "" );
        if ( cmdexp.captureCount() == 2 ) {
            cmd.m_param = cmdexp.cap( 2 );
        }
        log.stream() << "Emitting barcodeCommandScan( { m_code: '" << cmd.m_code << "', m_param: '" << cmd.m_param << "' } )";
        emit barcodeCommandScan( cmd );
        return;
    }

    // Now, could this be a SNN scan?
    QRegExp ssn_re( "\\d{10}"); // This could be configurable...
    if ( ssn_re.exactMatch( code ) ) {
        log.stream() << "Emitting barcodeSSNScan( '" << code << "' )";
        emit barcodeSSNScan( code );
        return;
    }

    // Nope, assuming an item scan.
    log.stream() << "Emitting barcodeItemScan( '" << code << "' )";
    emit barcodeItemScan( code );
}

void Interceptor::emitMagSwipe(const QRegExp &regExp, bool track1, bool track2, bool track3) const {
    Logger log("void Interceptor::tryEmitMagSwipe(const QRegExp &regExp) const");
    // It is assumed that the caller has confirmed match on the regexp, and that it matches the tracks.
    QString track1value;
    QString track2value;
    QString track3value;
    int captures = regExp.captureCount();
    int nextCapture = 1;

    if ( track1 && nextCapture <= captures ) {
        track1value = regExp.cap( nextCapture );
        nextCapture += 1;
    }
    if ( track2 && nextCapture <= captures ) {
        track2value = regExp.cap( nextCapture );
        nextCapture += 1;
    }
    if ( track3 && nextCapture <= captures ) {
        track3value = regExp.cap( nextCapture );
        nextCapture += 1;
    }
    log.stream() << "Number of captures : " << captures;
    log.stream() << "Track1 : '" << track1value << "'";
    log.stream() << "Track2 : '" << track2value << "'";
    log.stream() << "Track3 : '" << track3value << "'";
    if ( track1value.toUpper() == "E"
         || track2value.toUpper() == "E"
         || track3value.toUpper() == "E" ) {
        log.stream( warn ) << "Failed to read or more tracks";
        emit magSwipeFailure();
        return;
    }
    // Figure out if this is a DK Sundhedskort.
    if ( track1 && track2 && !track3 ) {
        log.stream() << "Checking 2 track (1,2) card";
        log.stream() << "Mabbe it is a DKSundhedskort";
        try {
            emit magSwipe( DKSundhedskort( track1value, track2value ) );
            return;
        } catch ( ... ) {
            log.stream( warn ) << "Probably not a DKSundhedskort";
        }
        log.stream() << "Mabbe it is a CreditCard";
        try {
            emit magSwipe( CreditCard( track1value, track2value ) );
            return;
        } catch ( ... ) {
            log.stream( warn ) << "Probably not a CreditCard";
        }
        emit magSwipeFailure();
        return;
    }

    // Reaching here is an error
    log.stream( warn ) << "Totalmatch, but no card match found.";
    emit magSwipeFailure();
}

void Interceptor::emitMagSwipe() const {
    Logger log( "void Interceptor::emitMagSwipe() const" );
    log.stream() << "EMITTING MAGSWIPE";
    QString totalMatch = magswipe_exp.cap( 0 );
    log.stream() << "Text matched: '" << totalMatch << "'";
    log.stream() << "Text matched in % encoding: '" << totalMatch.toLocal8Bit().toPercentEncoding() << "'";

    // Split up in tracks. Try to match all three tracks at once...
    // Note: I could not figure out how to do this cleverly.... :-(

    // Have to start with the most specific, that is, all three tracks.
    // When it fails badly, it will return data in all tracks...
    // Track 1, 2 and 3
    QRegExp all( QString( "%0(.+)%4%1(.+)%4%2(.+)%4%5" )
                 .arg( mag_track1start ).arg( mag_track2start ).arg( mag_track3start )
                 .arg( mag_trackEnd ).arg( mag_end ) );
    all.setCaseSensitivity( Qt::CaseInsensitive );
    if ( all.exactMatch( totalMatch ) ) {
        log.stream() << "Match on track 1, 2, 3";
        emitMagSwipe( all, true, true, true );
        return;
    }

    // Track 1 and 2
    all.setPattern( QString( "%0(.+)%3%1(.+)%3%4" )
                    .arg( mag_track1start ).arg( mag_track2start )
                    .arg( mag_trackEnd ).arg( mag_end ) );
    if ( all.exactMatch( totalMatch ) ) {
        log.stream() << "Match on track 1, 2 ";
        emitMagSwipe( all, true, true, false );
        return;
    }

    // No match, complain
    log.stream( warn ) << "Unable to understand card data '" << totalMatch << "'";
    emit magSwipeFailure();
}












