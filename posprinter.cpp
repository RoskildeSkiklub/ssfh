#include "posprinter.h"

/** \brief Basic support for ESC/Pos printer.
  *
  * This could be generalized, and whatnot, but I only really have
  * a single printer. So here goes. */

// app
#include "log.h"

using namespace Log;

namespace Pos {

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

        // Todo : cut, formfeed.
        return true;
    } else {
        log.stream( warn ) << "Device not available";
        return false;
    }
}

void Printer::startReceipt() {
    Logger log("void Printer::startReceipt()");
    m_buffer.clear();
}

void Printer::endReceipt() {
    Logger log("void Printer::endReceipt()");
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
    m_buffer.append( "<bold>" );
    addCloseModifier( "</bold>" );
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

/** \brief Endline
  *
  * Ends the current line */
Printer & endl( Printer & os ) {
    return os.endl();
};

}; // namespace
