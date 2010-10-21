#include "exception.h"

// qt
#include <QMessageBox>

QString Exception::toString() const {
    return QString( tr( "Exception: %1, '%2'" ) ).arg( m_statusCode ).arg( m_description );
}

void default_critical( Log::Logger & log, const QString & msg ) {
    log.stream( Log::error ) << "Caught exception, showing default_critical MessageBox to user. Exception msg is : '"
                << msg << "'";
    QMessageBox::critical( 0, Exception::tr("Unexcepted error"),
                           Exception::tr("<p><b>An unexpected error happened</b></p>"
                                    "<p>Please try one of the following workarounds:</p>"
                                    "<p><ul><li>Retry the operation</li>"
                                    "<li>Close, then reopen the dialog and try again</li>"
                                    "<li>Restart the application and try again</li></ul></p>"
                                    "<p>The error message was:<br><b>%1</b></p>"
                                    "<p>More information may be available in the log</p>")
                           .arg( msg ) );
}
