#include "feedbackobject.h"

// Qt
#include <Phonon/MediaObject>
#include <QApplication>
#include <QDir>
#include <QFile>

// App
#include "log.h"

using namespace Phonon;
using namespace Log;

//TODO This may leak files - but who cares... (valgrind does).

FeedbackObject::FeedbackObject(QObject *parent) :
    QObject(parent), m_media_object( 0 )
{
    Logger log( "FeedbackObject::FeedbackObject(QObject *parent)");
    log.stream() << "Copying sounds to temporary dir";

    // I can't make it work with ressources directly, and hardly with files...
    if ( ! QDir::temp().exists( "ssfhsounds" ) && ! QDir::temp().mkdir( "ssfhsounds" ) ) {
        log.stream( error ) << "Unable to create directory for sounds - no sound support";
        return;
    }
    QString targetDir = QDir::tempPath() + QDir::separator()
            + "ssfhsounds" + QDir::separator();

    // Copy all ressources
    log.stream() <<  "Creating sound objects";
    // Setup the media object - set actual sounds later.
    m_media_object
            = createPlayer( NotificationCategory );

    // Map EventTypes to file names.
    eventToFilename[ItemAdded]    = addSound( "item_added.wav", targetDir );
    eventToFilename[ItemReturned] = addSound( "item_added.wav", targetDir );
    eventToFilename[HirerSet]     = addSound( "hirer_set.wav", targetDir );
    eventToFilename[ItemScanned]  = addSound( "item_added.wav", targetDir );
}

QString FeedbackObject::addSound(const QString &ressourceName, const QString &targetDir ) {
    Logger log("void FeedbackObject::addSound(QString ressourceName)");
    log.stream() << "Trying to add sound from ressource sounds/'" << ressourceName
            << "' to targetDir '" << targetDir << "'";
    QString targetFile = targetDir + QDir::separator() + ressourceName;
    if ( QFile::exists( targetFile )
        || QFile::copy( QString( ":sounds/%0" ).arg( ressourceName ), targetFile ) ) {
        log.stream() << "Adding sound '" << ressourceName << "'";
        return targetFile;
    }
    return "";
}

FeedbackObject::~FeedbackObject() {
    Logger log("FeedbackObject::~FeedbackObject()");
    delete m_media_object;
    m_media_object = 0;
}

void FeedbackObject::eventTriggered(const EventType &type) const {
    Logger log("void FeedbackObject::eventTriggered(EventType &type)");
    if ( m_media_object ) {
        m_media_object->setCurrentSource( MediaSource( eventToFilename[type]) );
        m_media_object->play();
    } else {
        log.stream( warn ) << "No media object, not playing sounds";
    }
}

// Below here should go at some point, really.

void FeedbackObject::itemAdded() const {
    Logger log("FeedbackObject::itemAdded()");
    eventTriggered( ItemAdded );
}

void FeedbackObject::itemReturned() const {
    Logger log("FeedbackObject::itemReturned()");
    eventTriggered( ItemReturned );
}

void FeedbackObject::hirerSet() const {
    Logger log("FeedbackObject::hirerSet()");
    eventTriggered( HirerSet );
}

void FeedbackObject::itemScanned() const {
    Logger log("FeedbackObject::itemScanned()");
    eventTriggered( ItemScanned );
}
