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
    bool ignore_next_keyevent; // Set to true, when we post events ourselves.


    void postCollected(); // Post collected info. Called when no match.
    void emitBarcodeScan() const;
    void emitMagSwipe() const;

private slots:
    void timeout(); // Called by the timer, when it times out...
signals:
    void barcodeScan( const QString & id ) const;
    void magSwipe( const DKSundhedskort & dks ) const;

};

#endif // INTERCEPTOR_H
