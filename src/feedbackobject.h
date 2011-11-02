#ifndef FEEDBACKOBJECT_H
#define FEEDBACKOBJECT_H


// Qt
#include <QObject>
namespace Phonon {
    class MediaObject;
}


/** \brief Object to provide audio feedback on events */

class FeedbackObject : public QObject
{
    Q_OBJECT

    // item added sound
    Phonon::MediaObject * m_sound_item_added;
    Phonon::MediaObject * m_sound_hirer_set;

    // Ressourcename *without* sound
    void addSound( const QString & ressourceName,
                   const QString & targetDir,
                   Phonon::MediaObject ** media_object );

public:
    explicit FeedbackObject(QObject *parent = 0);
    ~FeedbackObject();

// Testing.
private slots:
    void testme();


signals:

public slots:
    /** \brief The sound of an item added to a contract */
    void itemAdded() const;
    /** \brief The sound of an item returned on a contract */
    void itemReturned() const;
    /** \brief The sound of an hirer set on a contract */
    void hirerSet() const;
    /** \brief The sound of an item scanned (general version of added, really) */
    void itemScanned() const;
};

#endif // FEEDBACKOBJECT_H
