#ifndef HIRER_H
#define HIRER_H

// qt
#include <QString>
#include <QSqlQuery>
#include <QCoreApplication>

// In memory representation of a hirer

class Hirer {
    Q_DECLARE_TR_FUNCTIONS(Hirer)
public:

    qlonglong m_id; // 64 bit signed
    QString m_memberId;
    QString m_ssn;
    QString m_firstName;
    QString m_lastName;
    QString m_streetAddress;
    QString m_zip;
    QString m_city;
    QString m_country;
    QString m_note;


    /** \brief Create a hirer instance present in the database
     * \param id The id of the hirer in the database
     * \return A Hirer instance with the values loaded from the database
     *
     * Note, the methods throw, if the hirer is not present in the database! */
    Hirer( qlonglong id );
    Hirer();

    /** \brief Check if this hirer is valid, that is, contains valid data
      * \return True if valid, false otherwise */
    bool isValid() const { return m_valid; };

    /** \brief Format as HTML for use in dialogs */
    QString toHtml() const { return "Not yet implemented"; };

private:
    /** \brief Holds the valid state */
    bool m_valid;

    /** \brief Create from an QSqlQuery
      * \param query The executed QSqlQuery
      *
      * This method creates a Hirer instance from the values in the QSqlQuery.
      * The query must be active and the orders of the colums must match the assumptions in the code. */
    Hirer( const QSqlQuery & query );



};

#endif // HIRER_H
