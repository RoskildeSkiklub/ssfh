#include "posimage.h"

// Qt
#include <QImage>

// App
#include "exception.h"
#include "log.h"

using namespace Log;
using namespace std;

namespace Pos {

    // Map to bytes.
    void Image::setPixel( int x, int y ) {
        size_t byte_index = x * m_gs_y + y / 8;
        int bit_no = y % 8;
        unsigned char bit = 1 << ( 7 - bit_no );
        // cout << "Setting bit " << bit_no << " in byte " << byte_index << endl;
        m_bytes.at( byte_index ) = m_bytes.at( byte_index ) | bit;
    }

    // Constructor. Set up the bytes.
    Image::Image( const QImage & image ) : m_valid( false ) {
        Logger log("Image::Image( const QImage & image )");
        // Set up x and y as pr. epson manual
        m_width = image.width();
        m_height = image.height();

        m_gs_x = m_width / 8 + (m_width % 8 != 0?1:0);
        m_gs_y = m_height / 8 + (m_height % 8 != 0?1:0);

        if ( m_gs_x > 255 || m_gs_y > 255 ) {
            throw Exception( Errors::PosImageTooLarge )
                    << ( log.stream( error )
                         << "The image passed to the constructor was to large in one or both dimensions." );
        }
        m_gs_k = m_gs_x * m_gs_y * 8;
        if ( m_gs_k > (3072*8) ) {
            throw Exception( Errors::PosImageTooLarge )
                    << ( log.stream( error )
                         << "The image passed to the constructor was to large in area." );
        }

        //        cout << "width: " << m_width << ", m_gs_x: " << m_gs_x << endl;
        // cout << "height: " << m_height << ", m_gs_y: " << m_gs_y << endl;
        // cout << "m_gs_k: " << m_gs_k << endl;

        vector<unsigned char> bytes( m_gs_k, 0 ); // Blank all bytes.
        m_bytes = bytes;

        for ( int i_y = 0; i_y < m_height; ++i_y ) {
            for ( int i_x = 0; i_x < m_width; ++i_x ) {
                //                 cout << "Pixel at " << i_x << ", " << i_y << " has color " << image.pixel( i_x, i_y ) << endl;
                if ( image.pixelIndex( i_x, i_y ) == Qt::color1 ) {
                    setPixel( i_x, i_y );
                }
            }
        }
        m_valid = true;
    }

    QByteArray Image::getGSStar() const {
        Logger log("QByteArray getGSStar() const");
        if ( m_valid ) {
            QByteArray res( m_bytes.size() + 4, 0 );
            res[0] = 29;
            res[1] = '*';
            res[2] = (unsigned char) m_gs_x;
            res[3] = (unsigned char) m_gs_y;
            for ( size_t i = 0; i < m_bytes.size(); ++i ) {
                res[ 4 + i ] = m_bytes.at( i );
            }
            return res;
        } else {
            return QByteArray();
        }
    };


}; // namespace
