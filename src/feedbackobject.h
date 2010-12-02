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

signals:

public slots:
    void itemAdded() const;
    void itemReturned() const;
    void hirerSet() const;

};

#endif // FEEDBACKOBJECT_H
