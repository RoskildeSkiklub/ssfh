/* * Snow Stuff For Hire - A POS-like system designed for very quick rental
 * operations at small ski resorts
 * Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
 *
 * This file is part of Snow Stuff For Hire.
 *
 * Snow Stuff For Hire is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Snow Stuff For Hire is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef POSIMAGE_H
#define POSIMAGE_H

// C++
#include <vector>

// Qt
#include <QByteArray>

class QImage;
namespace Pos {

/** \brief Declare an image abstraction for a pos bitmap */
class Image {
private:
    /** \brief If false, the image is not valid */
    bool m_valid;

    /** \brief Width of (source) image in pixels */
    int m_width;

    /** \brief Height of (source) image in pixels */
    int m_height;

    /** \brief Epson GS X parameter stored here */
    int m_gs_x;

    /** \brief Epson GS Y parameter stored here */
    int m_gs_y;

    /** \brief Epson GS K parameter stored here */
    int m_gs_k;

    /** \brief Actual bytes for image/output */
    std::vector<unsigned char> m_bytes;

    /** \brief Set a pixel in the image
      * \param x The x coordinate of the pixel to turn on
      * \param y The y coordinate of the pixel to turn on */
    void setPixel( int x, int y );

public:
    /** \brief Constructor
      * \param image The monochrome image to embed in this instance
      *
      * The image must be monochrome! */
    Image( const QImage & image );

    /** \brief Constructor
      *
      * This construct an invalid instance */
    Image() : m_valid( false ){}

    /** \brief Return whole GS Star command
      *
      * The data in the array returned, can be passed to a Pos printer,
      * and it will define the image in the printers memory. Use GS/0 to print it */
    QByteArray getGSStar() const;
};

}; // namespace

#endif // POSIMAGE_H
