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
