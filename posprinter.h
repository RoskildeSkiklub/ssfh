#ifndef POSPRINTER_H
#define POSPRINTER_H

/** \brief Printer interface class.
  *
  * A global instance of this is used when wanting to print something
  * to the pos/receipt printer. */

#include <QString>
#include <QFile>
#include <QByteArray>

class PosPrinter
{
public:
    /** \brief Constructor
     * \param dev The name of the device the printer is connected to
     *
     * The instance will keep trying to open the device whenever
     * something is printed, and will perform initialization
     * after opening the device file */
    PosPrinter( const QString & dev );

    /** \brief Emit a receipt to the printer device
      * \param data The data to print to the device
      *
      * The data should be formed by calling appropiate methods
      * and store the bytes.
      *
      * The data is printed, and the receipt pagefeed and cutted as
      * appropiate */
    bool emitReceipt( const QByteArray & data );

private:
    QString m_dev;
    QFile m_device_file;
    bool openDevice();

};

#endif // POSPRINTER_H
