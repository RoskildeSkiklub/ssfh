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

    /** \brief Indicates a failure to read a track, or failure to understand the format. */
    void magSwipeFailure() const;

};

#endif // INTERCEPTOR_H
