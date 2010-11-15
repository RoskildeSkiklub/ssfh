#ifndef __POS_PRINTER_H
#define __POS_PRINTER_H

// Qt
#include <QString>
#include <QFile>
#include <QByteArray>
class QImage;

// App
#include "posimage.h"

namespace Pos {

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
      * Note, that the combination of manipulators are limited.
      * E.g. you can not meaningfully center a hr, for instance.
      *
      * \code
      * PrinterInstance << "This is printed" << Printer::bold
      * << "This will be bold" << "This will not be bold" << Printer::endl
      * \endcode
      *
    */
class Printer {
public:
    /** \brief Constructor
     * \param dev The name of the device the printer is connected to
     *
     * The instance will keep trying to open the device whenever
     * something is printed, and will perform initialization
     * after opening the device file */
    Printer( const QString & dev );

    /** \brief Sets a logo
      * \param logo The logo to print, whenever the logo io manipulator is used
      *
      * Note, the image should be monochrome for best results */
    void setLogo( const QImage & logo );

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

    /** \brief Output a string
      *
      * Note, the string is converted to Latin1 before
      * beeing sent to the printer */
    Printer & operator<<( const QString & str );

    /** \brief Get the page/receipt width
      * \return The receipt with in characters */
    qlonglong getReceiptWidth() const;

    //////////////////////////////////////////////////////////////////////
    // Modifiers

    typedef Printer & (*iomanip) (Printer &);

    /** \brief Output a io manipulator */
    Printer & operator<<( iomanip iom ) {
        return iom( *this );
    }

    /** \brief Emit logo.
      *
      * Emits the previosly set/loaded logo */
    Printer & logo();

    /** \brief Bold text
      *
      * This is used to enable bold for the next input */
    Printer & bold();

    /** \brief Underline text
      *
      * This is used to enable underline for the next input */
    Printer & underline();

    /** \brief Black/white reverse text
      *
      * This is used to enable black/white reverse for the next input */
    Printer & reverse();

    /** \brief Center text
      *
      * This is used to enable center for the next input.
      * Note, that a Pos::Endl is implied, if not present in the buffer already, and
      * is also printed after the next string. So, do not follow with Pos::endl */
    Printer & center();

    /** \brief Emit a "horizontal ruler".
      *
      * This will emit a series of ----. A Pos::endl is implied, if not
      * already present in the buffer. */
    Printer & hr();

    /** \brief Endline
      *
      * Ends the current line */
    Printer & endl();

private:
    /** \brief Name of the device file used to access the device */
    QString m_dev;

    /** \brief File handle into device file */
    QFile m_device_file;

    /** The data to emit next, is collected in this buffer
      * This is mostly for testing purposes */
    QByteArray m_buffer;

    /** \brief Store stuff to add after the next QString statement.
     *
     * This  is added in a stack like filo fashion. */
    QByteArray m_modifierclose;

    /** \brief The logo file. If set */
    Image m_logo;

    /** \brief Blank line flag.
      *
      * When set to true, the current line only contains nothing or control codes */
    bool m_blank_line_flag;

    /** \brief Vertical character width */
    unsigned char m_char_width;

    /** \brief Vertical character height */
    unsigned char m_char_height;


    /** \brief Method to add a closing modifier to the "stack"
      * of modifiers.
      *
      * This adds the modifier passed to in the beginning of modifierclose */
    void addCloseModifier( const QByteArray & closing );

    /** \brief Ensure current line is blank (only holds control stuff)
      *
      * Will insert a \n if needed. */
    void ensureBlank();

    /** \brief Make sure we can talk to the device.
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

/** \brief Underline text
  *
  * This is used to enable underline for the next input */
Printer & underline( Printer & os );

/** \brief Black white reverse text
  *
  * This is used to enable reverse black/white for the next input */
Printer & reverse( Printer & os );

/** \brief Center text
  *
  * This is used to center text for the next input. A Pos::endl is implied, if not
  * already present in the buffer. */
Printer & center( Printer & os );

/** \brief Emit a "horizontal ruler".
  *
  * This will emit a series of ----. A Pos::endl is implied, if not
  * already present in the buffer. */
Printer & hr( Printer & os );

/** \brief Endline
  *
  * Ends the current line */
Printer & endl( Printer & os );

/** \brief Logo
  *
  * This is used to emit a logo */
Printer & logo( Printer & os );

}; // namespace

#endif // Printer_H
