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
    QObject(parent), m_sound_item_added( 0 ), m_sound_hirer_set( 0 )
{
    Logger log( "FeedbackObject::FeedbackObject(QObject *parent)");
    log.stream() << "Copying sounds to temporary dir";

    // I can't make it work with ressources directly, and hardly with files...
    if ( ! QDir::temp().exists( "ssfhsounds" ) && ! QDir::temp().mkdir( "ssfhsounds" ) ) {
        log.stream( error ) << "Unable to create directory for sounds - no sound support";
        return;
    }
    QString targetDir = QDir::tempPath() + QDir::separator() + "ssfhsounds" + QDir::separator();

    // Copy all ressources
    log.stream() <<  "Creating sound objects";
    addSound( "item_added.wav", targetDir, &m_sound_item_added );
    addSound( "hirer_set.wav", targetDir, &m_sound_hirer_set );
    connect( m_sound_item_added, SIGNAL(finished()), this, SLOT(testme()));

}

void FeedbackObject::addSound(const QString &ressourceName, const QString &targetDir,
                              Phonon::MediaObject **media_object) {
    Logger log("void FeedbackObject::addSound(QString ressourceName)");
    log.stream() << "Trying to add sound from ressource sounds/'" << ressourceName
            << "' to targetDir '" << targetDir << "'";
    QString targetFile = targetDir + QDir::separator() + ressourceName;
    if ( QFile::exists( targetFile )
        || QFile::copy( QString( ":sounds/%0" ).arg( ressourceName ), targetFile ) ) {
        log.stream() << "Adding sound '" << ressourceName << "' as a MediaSource";
        *media_object
                = createPlayer( NotificationCategory, MediaSource( targetFile ) );
        // This really is a bug, somewhere....
        // (*media_object)->play();
    }
}

void FeedbackObject::testme() {
    Logger log( "void FeedbackObject::testme()" );
    // m_sound_item_added->seek(0);
}


FeedbackObject::~FeedbackObject() {
    Logger log("FeedbackObject::~FeedbackObject()");
    delete m_sound_item_added;
    m_sound_item_added = 0;
    delete m_sound_hirer_set;
    m_sound_hirer_set = 0;
}

void FeedbackObject::itemAdded() const {
    Logger log("FeedbackObject::itemAdded()");
    if ( m_sound_item_added ) {
        log.stream() << "Playing sound";
        m_sound_item_added->setCurrentSource( MediaSource( "/tmp/ssfhsounds/item_added.wav") );
        // m_sound_item_added->seek(0);
        m_sound_item_added->play();
    } else {
        log.stream() << "Not playing sound... ";
    }
}

void FeedbackObject::itemReturned() const {
    Logger log("FeedbackObject::itemReturned()");
    if ( m_sound_item_added ) { // TODO: Change...
        m_sound_item_added->play();
    }
}

void FeedbackObject::hirerSet() const {
    Logger log("FeedbackObject::hirerSet()");
    if ( m_sound_hirer_set ) {
        m_sound_hirer_set->play();
    }
}

void FeedbackObject::itemScanned() const {
    Logger log("FeedbackObject::itemScanned()");
    if ( m_sound_item_added ) { // TODO: Change...
        m_sound_item_added->play();
    }
}
