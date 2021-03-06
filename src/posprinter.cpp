//
// Snow Stuff For Hire - A POS-like system designed for very quick rental
// operations at small ski resorts
// Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
//
// This file is part of Snow Stuff For Hire.
//
// Snow Stuff For Hire is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Snow Stuff For Hire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
//

#include "posprinter.h"

/** \brief Basic support for ESC/Pos printer.
  *
  * This could be generalized, and whatnot, but I only really have
  * a single printer. So here goes. */

// Qt
#include <QImage>
#include <QDateTime>
#include <QDate>

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


Printer::Printer( const QString & dev )
    : m_dev( dev ), m_blank_line_flag( true ), m_font( FontA ),
    m_char_width( 1 ), m_char_height( 1 ), m_logo_transferred( false ) {
    Logger log("Printer::Printer()");
    m_device_file.setFileName( dev );
}

bool Printer::ready() {
    Logger log("bool Printer::ready()");
    return openDevice();
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
    m_logo_transferred = false;
}

void Printer::setFont( Font font) {
    Logger log("void Printer::setFont(unsigned char font)");
    if ( font < 0 || font > 2 ) {
        throw Exception( Errors::PosPrinterIllegalFont )
                << ( log.stream( error )
                     << "Illegal font '" << font << "' specified." );
    }
    m_font = font;
    m_buffer.append( ESC "M" );
    m_buffer.append( static_cast<unsigned char>( font ) );
}

void Printer::setFontSize(unsigned char width, unsigned char height) {
    Logger log("void Printer::setFontSize(unsigned char width, unsigned char height)");
    if ( width < 1 || width > 8 ) {
        throw Exception( Errors::PosPrinterIllegalFontSize )
                << ( log.stream( error )
                     << "Illegal font width '" << width << "' specified." );
    }
    if ( height < 1 || width > 8 ) {
        throw Exception( Errors::PosPrinterIllegalFontSize )
                << ( log.stream( error )
                     << "Illegal font height '" << height << "' specified." );
    }
    m_char_width = width;
    m_char_height = height;
    unsigned char res = 16 * ( m_char_width - 1 ) + ( m_char_height -1 );
    m_buffer.append( GS "!" );
    m_buffer.append( res );
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
    };
    return true;
}

void Printer::closeDevice() {
    Logger log("void Printer::closeDevice()");
    m_device_file.close();
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
    // Reset the printer ...
    m_device_file.write( ESC "@" );
    m_logo_transferred = false;
    m_device_file.write( setLatin1 );

    // Set the font and fontsize, then flush the buffer
    setFont( m_font );
    setFontSize( m_char_width, m_char_height );
    m_device_file.write( m_buffer );
    m_buffer.clear();
}

void Printer::transferLogo() {
    Logger log("void Printer::transferLogo()");
    if ( ! m_logo_transferred ) {
        m_device_file.write( m_logo.getGSStar() );
        m_logo_transferred = true;
    }
};

void Printer::startReceipt() {
    Logger log("void Printer::startReceipt()");
    m_buffer.clear();
    openDevice();
    setupPrinter();
    m_blank_line_flag = true;
}

void Printer::endReceipt() {
    Logger log("void Printer::endReceipt()");
    m_buffer.append( QByteArray( "\n\n\n\n") + doCut );
    emitReceipt( m_buffer );
    m_buffer.clear();
    m_blank_line_flag = true;
    closeDevice();
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
    switch( m_font ) {
    case FontA: return 576 / 12 / m_char_width;
    case FontB: return 576 / 9 / m_char_width;
    // case FontC: return 576 / 8 / m_char_width;
    default:
        throw Exception( Errors::InternalError )
                << ( log.stream(error)
                     << "Unknown font size in Pos::Printer" );
    }
}

////////////////////////////////////////////////////////////////////////////////////
// Output and modifier support.

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

Printer & Printer::operator <<( const QDateTime & dt ) {
    Logger log("Printer & Printer::operator <<( const QDateTime & dt )");
    return this->operator <<( dt.toString( "yyyy-MM-dd hh:mm" ) );
}

Printer & Printer::operator <<( const QDate & date ) {
    Logger log("Printer & Printer::operator <<( const QDate & date )");
    return this->operator <<( date.toString( "yyyy-MM-dd" ) );
}

Printer & Printer::operator <<( const Barcode & barcode ) {
    Logger log("printer & Printer::operator <<( const Barcode & barcode )");
    if ( barcode.getBarcode().size() > 10 ) {
        log.stream( error ) << "Unable to print barcode '" << barcode.getBarcode() << "' because it is too long.";
        return *this;
    }
    ensureBlank();
    // Set height, and id below barcode
    m_buffer.append( QByteArray( GS "hA" GS "H2" ) );
    m_buffer.append( GS "kE" ); // Used code 39
    m_buffer.append( static_cast<unsigned char>( barcode.getBarcode().size() ) );
    return this->operator<<( barcode.getBarcode() );
}

Printer & Printer::logo() {
    Logger log("Printer & Printer::logo()");
    transferLogo();
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
    m_buffer.append( QString( getReceiptWidth() , '-' ) );
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
