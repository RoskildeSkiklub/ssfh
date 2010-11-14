#include "posprinter.h"

/** \brief Basic support for ESC/Pos printer.
  *
  * This could be generalized, and whatnot, but I only really have
  * a single printer. So here goes. */

// app
#include "log.h"
#include "exception.h"

using namespace Log;


namespace Pos {

    /** This is just easiest to handle with defines */
    #define ESC "\033"
    #define FS "\034"
    #define GS "\035"

    // Something that makes the pos-104 to support only a single encoding
    const QByteArray ml0( FS"." );

    // Codepage 6 of POS-104 is Latin1
    const QByteArray setLatin1( ml0 + QByteArray( ESC "t\006" ) );

    const QByteArray boldOn( ESC "E\001" );
    const QByteArray boldOff( ESC "E\000", 3 );

    const QByteArray underlineOn( ESC "-\001" );
    const QByteArray underlineOff( ESC "-\000", 3 );

    const QByteArray cut( GS "V1" );

Printer::Printer( const QString & dev ) : m_dev( dev ){
    Logger log("Printer::Printer()");
    m_device_file.setFileName( dev );
}

bool Printer::openDevice() {
    Logger log("Printer::openDevice()");
    // TODO: Set up graphics, codepages, stuff like that.
    if ( ! ( m_device_file.isOpen() && m_device_file.isWritable() ) ) {
        log.stream() << "m_device_file '" << m_dev
                << "' not open/writeable, trying to open";
        m_device_file.close();
        m_device_file.open( QIODevice::WriteOnly );
        if ( ! ( m_device_file.isOpen() && m_device_file.isWritable() ) ) {
            log.stream( warn ) << "m_device_file '" << m_dev
                    << "' still not open/writeable";
            return false;
        }
    };
    return true;
}

bool Printer::emitReceipt( const QByteArray & data ) {
    Logger log("bool Printer::emitReceipt()");
    if ( openDevice() ) {
        log.stream() << "Outputting receipt";
        m_device_file.write( data );
        m_device_file.write( "\n" );
        m_device_file.flush();
        return true;
    } else {
        log.stream( warn ) << "Device not available";
        return false;
    }
}

void Printer::setupPrinter() {
    Logger log("void setupPrinter()");
    if ( ! ( m_device_file.isOpen() && m_device_file.isWritable() ) ) {
        throw Exception( Errors::PosPrinterNotOpen )
                << ( log.stream( error )
                     << "setupPrinter called, but device is not open/writeable.");
    }
    m_device_file.write( setLatin1 );
}

void Printer::startReceipt() {
    Logger log("void Printer::startReceipt()");
    m_buffer.clear();
}

void Printer::endReceipt() {
    Logger log("void Printer::endReceipt()");
    m_buffer.append( QByteArray( "\n\n\n\n") + cut );
    emitReceipt( m_buffer );
}

Printer & Printer::operator <<( const QString & str ) {
    Logger log( "Printer & Printer::operator <<( const QString & str )" );
    m_buffer.append( str.toLatin1() );
    m_buffer.append( m_modifierclose );
    m_modifierclose.clear();
    return *this;
}

Printer & Printer::bold() {
    Logger log("Printer & Printer::bold()");
    m_buffer.append( boldOn );
    addCloseModifier( boldOff );
    return *this;
}

Printer & Printer::underline() {
    Logger log("Printer & Printer::bold()");
    m_buffer.append( underlineOn );
    addCloseModifier( underlineOff );
    return *this;
}

Printer & Printer::endl() {
    Logger log("Printer & Printer::endl(Printer &os)");
    m_buffer.append( "\n" );
    return *this;
}


void Printer::addCloseModifier(const QByteArray &closing) {
    Logger log("void Printer::addCloseModifier(const QByteArray &closing)");
    m_modifierclose.prepend( closing );
}

/** \brief Bold text
  *
  * This is used to enable bold for the next input */
Printer & bold( Printer & os ) {
    return os.bold();
};

/** \brief Underline text
  *
  * This is used to enable underline for the next input */
Printer & underline( Printer & os ) {
    return os.underline();
};

/** \brief Endline
  *
  * Ends the current line */
Printer & endl( Printer & os ) {
    return os.endl();
};

}; // namespace
