#include "posprinter.h"

// app
#include "log.h"

using namespace Log;

PosPrinter::PosPrinter( const QString & dev ) : m_dev( dev ){
    Logger log("PosPrinter::PosPrinter()");
    m_device_file.setFileName( dev );
}

bool PosPrinter::openDevice() {
    Logger log("PosPrinter::openDevice()");
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
        return true;
    } else {
        log.stream( warn ) << "Device not available";
        return false;
    }
}
