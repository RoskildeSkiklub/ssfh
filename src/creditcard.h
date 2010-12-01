#ifndef CREDITCARD_H
#define CREDITCARD_H

// Qt
#include <QString>

/** \file
  * \brief Class that represents data read from the magnetic stripe on a credit card */
class CreditCard
{
public:

    /** \brief Construct from two tracks
     * \param track1 Data in track1
     * \param track2 Data in track2
     *
     * Will throw, if no match */

    CreditCard(const QString & track1, const QString & track2 );

    QString name;
    QString surname;
    QString cardnumber;

};

#endif // CREDITCARD_H
