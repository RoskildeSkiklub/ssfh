#ifndef QUSB_H
#define QUSB_H
 
// This file posted without license information at
// http://www.microchip.com/forums/tm.aspx?m=340892&mpage=2
// Original author "skaman"

// I have added the public method "isDevicePresent" with datatype.

#include <QObject>
#include <QtGlobal>
 
// libUsb. I think
#include <usb.h>
 
namespace Usb {
    struct Id {
        quint16 vendorId;
        quint16 productId;
        Id( quint16 _vendorId, quint16 _productId )
            : vendorId( _vendorId ), productId( _productId ) {};
    };


 class QUSB : public QObject
 {
 private:
     const static int        INTERFACE = 0;
     const static int        CONFIGURATION = 1;
     const static int        ENDPOINT_IN = 0x81;
     const static int        ENDPOINT_OUT = 0x01;
     const static int        TIMEOUT = 5000;
 
     bool                    m_IsOpened;
     usb_dev_handle          *m_UsbDevice;
 
     bool FindDevice (quint16 VendorId, quint16 ProductId);
 
 public:
     QUSB();
 
     bool Open (quint16 VendorId, quint16 ProductId);
     void Close ();
     bool Write (char *packet, quint8 size);
     bool Read (char *packet, quint8 size);

     /** \brief Return wheter a given vendor/product combo is present
      * \param vendorId Id of the vendor
      * \param productId Id of the product */
     static bool isDevicePresent( quint16 vendorId, quint16 productId );
     /** \overload */
     static bool isDevicePresent( const Id & id );

 };

} //namespace
 
 #endif // QUSB_H
