#ifndef DKSUNDHEDSKORT_H
#define DKSUNDHEDSKORT_H

/** \file
  * \brief Class that - sort of - knows how to get from a magnetic wipe to a sundhedskort */

#include <QRegExp>

class DKSundhedskort {
public:
    /** \brief Construct from a matching regexp
      * \param regexp Regexp matching the static regexp */
    DKSundhedskort( const QRegExp & regexp );

    /** \brief Return a regexp suitable for matching a mag card swipe
      * \returns A regexp suitable for matching a mag card swipe */
    static QRegExp getRegExp() {
        return regexp;
    }

    QString name;
    QString surname;
    QString address;
    QString zip;
    QString city;
    // Always Danmark:
    QString country;
    QString ssn;

private:
    static QRegExp regexp;

    // TODO: Would be great with some unit tests on these functions.

    /** \brief Fixes up Danish Letters in the sundhedskort
      * \param input The string with weird chars
      * \return The fixed up string with danish letters in
      *
      * The danish letters are encoded according to iso 7811-2, but
      * the card reader/xwindows does not get them right. This
      * tries to dix them up */
    QString fixUpDanishLetters( const QString & input ) const;

    /** \brief Capitalize the first letter in each word
      * \param input The string to change
      * \return The string with each word capitalized first letter
      *
      * This changes e.g. MADS BONDO to Mads Bondo. The card reader can only
      * return capital letters. */
    QString capitalizeWords( const QString & input ) const;

};

#endif // DKSUNDHEDSKORT_H
