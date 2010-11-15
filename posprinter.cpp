#include "posprinter.h"

/** \brief Basic support for ESC/Pos printer.
  *
  * This could be generalized, and whatnot, but I only really have
  * a single printer. So here goes. */

// Qt
#include <QImage>

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

    const QByteArray setAlignLeft( ESC "a0");
    const QByteArray setAlignCenter( ESC "a1");
    const QByteArray setAlignRight( ESC "a2");

    const QByteArray setBoldOn( ESC "E1" );
    const QByteArray setBoldOff( ESC "E0" );

    const QByteArray setUnderlineOn( ESC "-1" );
    const QByteArray setUnderlineOff( ESC "-0" );

    const QByteArray setReverseOn( GS "B1" );
    const QByteArray setReverseOff( GS "B0" );

    const QByteArray doCut( GS "V1" );
    const QByteArray logoOut( GS "/0", 3 );

Printer::Printer( const QString & dev ) : m_dev( dev ), m_blank_line_flag( true ) {
    Logger log("Printer::Printer()");
    m_device_file.setFileName( dev );
}

void Printer::setLogo(const QImage &logo) {
    Logger log("void Printer::setLogo(const QImage &logo)");
    QImage monoImage = logo.convertToFormat( QImage::Format_Mono, Qt::MonoOnly );
    if ( monoImage.isNull() ) {
        throw Exception( Errors::PosImageUnableToConvert )
                << ( log.stream(error)
                     << "Unable to convert image to monochrome for use with Pos printer" );
    }
    m_logo = Image( monoImage );
}

bool Printer::openDevice() {
    Logger log("Printer::openDevice()");
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
        setupPrinter();
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
    m_device_file.write( m_logo.getGSStar() );
    // TODO: Upload a logo.
}

void Printer::startReceipt() {
    Logger log("void Printer::startReceipt()");
    m_buffer.clear();
    m_blank_line_flag = true;
}

void Printer::endReceipt() {
    Logger log("void Printer::endReceipt()");
    m_buffer.append( QByteArray( "\n\n\n\n") + doCut );
    emitReceipt( m_buffer );
    m_blank_line_flag = true;
}

void Printer::ensureBlank() {
    Logger log("void Printer::ensureBlank()");
    if ( ! m_blank_line_flag ) {
        m_buffer.append( '\n' );
        m_blank_line_flag = true;
    }
}

qlonglong Printer::getReceiptWidth() const {
    Logger log("qlonglong Printer::getReceiptWidth() const");
    return 48;
}

////////////////////////////////////////////////////////////////////////////////////
// Modifier support.

Printer & Printer::operator <<( const QString & str ) {
    Logger log( "Printer & Printer::operator <<( const QString & str )" );
    m_buffer.append( str.toLatin1() );
    m_buffer.append( m_modifierclose );
    // Update blank flag
    if ( !str.isEmpty() ) {
        m_blank_line_flag = ( str.at( str.size() - 1 ) == '\n' );
    }
    if ( ! m_blank_line_flag && m_modifierclose.contains('\n') ) {
        m_blank_line_flag = true;
    }
    m_modifierclose.clear();
    return *this;
}

Printer & Printer::logo() {
    Logger log("Printer & Printer::logo()");
    m_buffer.append( setAlignCenter );
    m_buffer.append( logoOut );
    m_buffer.append( setAlignLeft );
    m_blank_line_flag = true;
    return *this;
}

Printer & Printer::bold() {
    Logger log("Printer & Printer::bold()");
    m_buffer.append( setBoldOn );
    addCloseModifier( setBoldOff );
    return *this;
}

Printer & Printer::underline() {
    Logger log("Printer & Printer::underline()");
    m_buffer.append( setUnderlineOn );
    addCloseModifier( setUnderlineOff );
    return *this;
}

Printer & Printer::reverse() {
    Logger log("Printer & Printer::reverse()");
    m_buffer.append( setReverseOn );
    addCloseModifier( setReverseOff );
    return *this;
}

Printer & Printer::center() {
    Logger log("Printer & Printer::center()");
    ensureBlank();
    m_buffer.append( setAlignCenter );
    addCloseModifier( QByteArray( "\n" ) + setAlignLeft );
    // m_center_flag = true;
    return *this;
}

Printer & Printer::hr() {
    Logger log("Printer & Printer::hr()");
    ensureBlank();
    m_buffer.append( QString( getReceiptWidth(), '-' ) );
    m_buffer.append( "\n" );
    m_blank_line_flag = true;
    return *this;
}

Printer & Printer::endl() {
    Logger log("Printer & Printer::endl(Printer &os)");
    m_buffer.append( "\n" );
    m_blank_line_flag = true;
    return *this;
}


void Printer::addCloseModifier(const QByteArray &closing) {
    Logger log("void Printer::addCloseModifier(const QByteArray &closing)");
    m_modifierclose.prepend( closing );
}

////////////////////////////////////////////////////////////////////////
// Namespace functions.

Printer & logo( Printer & os ) {
    return os.logo();
};

Printer & bold( Printer & os ) {
    return os.bold();
};

Printer & underline( Printer & os ) {
    return os.underline();
};

Printer & reverse( Printer & os ) {
    return os.reverse();
};

Printer & center( Printer & os ) {
    return os.center();
};

Printer & hr( Printer & os ) {
    return os.hr();
};

Printer & endl( Printer & os ) {
    return os.endl();
};

}; // namespace
