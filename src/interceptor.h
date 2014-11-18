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

#ifndef INTERCEPTOR_H
#define INTERCEPTOR_H
/** \file
  * \brief A class to intercept certain events, and pass the on to Detectors */


// Qt
#include <QObject>
#include <QTimer>
#include <QRegExp>

class QEvent;
class DKSundhedskort;
class CreditCard;
namespace Globals {
    namespace BarcodeCommands {
        class Command;
    }
};

/** \brief Class to intercept keyboard events, and possibly translate to barcode/magnetic swipe events.
  *
  * This class intercepts any keyboard events. It looks for patterns that indicates that
  * a magnetic swipe, or a barcode scan (both: scan event) was introduced into the keyboard event queue. Keyboard events arrive
  * one of a time. This class works sort of like a DFA on the events, buffering the characterstring in hope of
  * matching an event, releasing them again (one by one), when it is clear that a scan event did not happen. If it
  * figures a scan event did happen, it emits a suitable signal. */
class Interceptor : public QObject {
    Q_OBJECT
public:
    Interceptor();
    /** \brief eventFilter override, installed for QApplication */
    bool eventFilter(QObject *obj, QEvent *ev);
private:
    QRegExp barcode_exp;
    QRegExp magswipe_exp;
    bool collecting; // Are we collecting inputs
    QString collected; // The currently collected string;
    QTimer timer; // Used to timeout non-matching typing
    QObject * last_obj; // Pointer to last obj that generated a key event.
    QString next_text_ignore; // Set to something, when we wish to ignore.

    void postCollected(); // Post collected info. Called when no match.

    /** \brief Called when a barcode has been scanned, and a signal needs emitting.
      *
      * This method emits one of the barcodeItemScan or
      * barcodeCommandScan signals as appropiate */
    void emitBarcodeScan() const;

    void emitMagSwipe() const;
    void emitMagSwipe( const QRegExp & regExp,
                          bool track1,
                          bool track2,
                          bool track3 ) const;

    void barcodeScan( const QString & code ) const;


private slots:
    void timeout(); // Called by the timer, when it times out...


signals:

    /** \brief An item was scanned with the barcode reader
      * \param id The id of the scanned item
      *
      * This signal indicates that an item was scanned */
    void barcodeItemScan( const QString & id ) const;

    /** \brief A SSN was scanned with the barcode reader
      * \param ssn The code of the scanned SSN
      *
      * This signal indicates that a SSN was scanned */
    void barcodeSSNScan( const QString & ssn ) const;

    /** \brief A command was scanned with the barcode reader
      * \param command The command that was scanned
      *
      * This signal indicates that a command was scanned */
    void barcodeCommandScan( const Globals::BarcodeCommands::Command & command ) const;

    /** \brief A magnetic DKSundhedskort was read with the magtek reader
      * \param dks The social security card read
      *
      * This signals that a danish security card was read */
    void magSwipe( const DKSundhedskort & dks ) const;

    /** \brief A magnetic CreditCard was read with the magtek reader
      * \param creditCard The creditcard read
      *
      * This signals that a danish security card was read */
    void magSwipe( const CreditCard & creditCard ) const;


    /** \brief Indicates a failure to read a track, or failure to understand the format. */
    void magSwipeFailure() const;

};

#endif // INTERCEPTOR_H
