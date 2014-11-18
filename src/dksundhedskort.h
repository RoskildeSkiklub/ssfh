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

#ifndef DKSUNDHEDSKORT_H
#define DKSUNDHEDSKORT_H

/** \file
  * \brief Class that - sort of - knows how to get from a magnetic wipe to a sundhedskort */

#include <QRegExp>

class DKSundhedskort {
public:
    /** \brief Construct from two tracks
      * \param track1 Data in track1
      * \param track2 Data in track2
      *
      * Will throw, if no match */
    DKSundhedskort( const QString & track1, const QString & track2 );

    QString name;
    QString surname;
    QString address;
    QString zip;
    QString city;
    // Always Danmark:
    QString country;
    QString ssn;

private:
    // TODO: Would be great with some unit tests on these functions.

    /** \brief Fixes up Danish Letters in the sundhedskort
      * \param input The string with weird chars
      * \return The fixed up string with danish letters in
      *
      * The danish letters are encoded according to iso 7811-2, but
      * the card reader/xwindows does not get them right. This
      * tries to fix them up */
    QString fixUpDanishLetters( const QString & input ) const;
};

#endif // DKSUNDHEDSKORT_H
