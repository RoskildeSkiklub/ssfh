#include "posprinter.h"

/** \brief Basic support for ESC/Pos printer.
  *
  * This could be generalized, and whatnot, but I only really have
  * a single printer. So here goes. */

// app
#include "log.h"

using namespace Log;

PosPrinter::PosPrinter( const QString & dev ) : m_dev( dev ){
    Logger log("PosPrinter::PosPrinter()");
    m_device_file.setFileName( dev );
}

bool PosPrinter::openDevice() {
    Logger log("PosPrinter::openDevice()");
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

bool PosPrinter::emitReceipt( const QByteArray & data ) {
    Logger log("bool PosPrinter::emitReceipt()");
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

void PosPrinter::startReceipt() {
    Logger log("void PosPrinter::startReceipt()");
    m_buffer.clear();
}

void PosPrinter::endReceipt() {
    Logger log("void PosPrinter::endReceipt()");
    emitReceipt( m_buffer );
}

PosPrinter & PosPrinter::operator <<( const QString & str ) {
    Logger log( "PosPrinter & PosPrinter::operator <<( const QString & str )" );
    m_buffer.append( str.toLatin1() );
    m_buffer.append( m_modifierclose );
    m_modifierclose.clear();
    return *this;
}

PosPrinter & PosPrinter::bold(PosPrinter &os) {
    Logger log("PosPrinter & PosPrinter::bold(PosPrinter &os)");
    os.m_buffer.append( "<bold>" );
    os.addCloseModifier( "</bold>" );
    return os;
}

PosPrinter & PosPrinter::endl(PosPrinter &os) {
    Logger log("PosPrinter & PosPrinter::endl(PosPrinter &os)");
    os.m_buffer.append( "\n" );
    return os;
}


void PosPrinter::addCloseModifier(const QByteArray &closing) {
    Logger log("void PosPrinter::addCloseModifier(const QByteArray &closing)");
    m_modifierclose.prepend( closing );
}
