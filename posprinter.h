#ifndef __POS_PRINTER_H
#define __POS_PRINTER_H


#include <QString>
#include <QFile>
#include <QByteArray>

/** \brief Printer interface class.
  *
  * A global instance of this is used when wanting to print something
  * to the pos/receipt printer.
  *
  *
  * Printing is statefull. You start printing by calling
  * startReceipt.
  * You pass stuff to be printed by using operator<<
  * Only QString and some custom io manipulators are supported.

  * By default, the printer understands ascii. The class initializes
  * it to use Latin1. (Because it needs to understand Danish).
  *
  * This means, that any strings you pass to it, must be able to be
  * converted to Latin1.
  *
  * To enable/disable a printer feature, you use custom io manipulators.
  *
  * \code
  * PrinterInstance << "This is printed" << Printer::bold
  * << "This will be bold" << Printer::endl
  * \endcode
  *
*/

namespace Pos {

class Printer
{
public:
    /** \brief Constructor
     * \param dev The name of the device the printer is connected to
     *
     * The instance will keep trying to open the device whenever
     * something is printed, and will perform initialization
     * after opening the device file */
    Printer( const QString & dev );

    /** \brief Start a new receipt
      *
      * This method starts a new receipt.
      * Nothing is actully printed, until
      * endReceipt is called */
    void startReceipt();

    /** \brief End the current receipt
      *
      * This method prints the buffered receipt. */
    void endReceipt();

    /** \brief Bold text
      *
      * This is used to enable bold for the next input */
    Printer & bold();

    /** \brief Underline text
      *
      * This is used to enable underline for the next input */
    Printer & underline();

    /** \brief Endline
      *
      * Ends the current line */
    Printer & endl();


    /** \brief Output a string
      *
      * Note, the string is converted to Latin1 before
      * beeing sent to the printer */
    Printer & operator<<( const QString & str );

    typedef Printer & (*iomanip) (Printer &);

    /** \brief Output a io manipulator */
    Printer & operator<<( iomanip iom ) {
        return iom( *this );
    }

private:
    QString m_dev;
    QFile m_device_file;

    /** The data to emit next, is collected in this buffer
      * This is mostly for testing purposes */
    QByteArray m_buffer;

    /** \brief Store stuff to add after the next QString statement */
    QByteArray m_modifierclose;

    /** \brief Method to add a closing modifier to the "stack"
      * of modifiers.
      *
      * This adds the modifier passed to in the beginning of modifierclose */
    void addCloseModifier( const QByteArray & closing );

    /* \brief Make sure we can talk to the device.
     *
     * This sets up the m_device_file to talk to the device.
     * It should be safe to call this function for every page
     * the client wants to print to the printer */
    bool openDevice();

    /** \brief Set up the printer
      *
      * This method is called by openDevice to set up the printer
      * after each openDevice. It downloads e.g. images, sets the
      * codepage, and stuff like that. It writes directly
      * to m_device_file, it does not buffer stuff. */
    void setupPrinter();

    /** \brief Emit a receipt to the printer device
      * \param data The data to print to the device
      *
      * The data should be formed by calling appropiate methods
      * and store the bytes.
      *
      * The data is printed, and the receipt pagefeed and cutted as
      * appropiate */
    bool emitReceipt( const QByteArray & data );


};

/** \brief Bold text
  *
  * This is used to enable bold for the next input */
Printer & bold( Printer & os );

/** \brief Endline
  *
  * Ends the current line */
Printer & endl( Printer & os );

}; // namespace

#endif // Printer_H
