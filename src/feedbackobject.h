#ifndef FEEDBACKOBJECT_H
#define FEEDBACKOBJECT_H

// Qt
#include <QObject>
#include <QMap>

// TODO: Migrate to QMultiMedia something. Right now, just eliminated everything, and
// calls mplayer... 

/** \brief Object to provide audio feedback on events */
class FeedbackObject : public QObject
{
    Q_OBJECT

public:
    explicit FeedbackObject(QObject *parent = 0);
    ~FeedbackObject();


    /** \brief The different types of event that gives feedback */
    typedef enum {
        ItemAdded = 0,
        ItemReturned = 1,
        HirerSet = 2,
        ItemScanned = 3
    } EventType;

public slots:
    /** Feedback on specific event
     * \param type The type of the event */
    void eventTriggered( const EventType & type ) const;

    // The rest here is actually deprecated.

    /** \brief The sound of an item added to a contract */
    void itemAdded() const;
    /** \brief The sound of an item returned on a contract */
    void itemReturned() const;
    /** \brief The sound of an hirer set on a contract */
    void hirerSet() const;
    /** \brief The sound of an item scanned (general version of added, really) */
    void itemScanned() const;

private:
    // Ressourcename *without* sound
    QString addSound( const QString & ressourceName,
                      const QString & targetDir );

    // Map eventtypes to filenames
    QMap<EventType,QString> eventToFilename;
};

#endif // FEEDBACKOBJECT_H
