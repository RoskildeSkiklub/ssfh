#include "qusb.h"

// This file posted without license information at
// http://www.microchip.com/forums/tm.aspx?m=340892&mpage=2
// Original author "skaman"


//app
#include "log.h"

using namespace Log;
 
namespace Usb {

 QUSB::QUSB()
 {
     this->m_IsOpened = false;
 }
 
 bool QUSB::FindDevice(quint16 VendorId, quint16 ProductId)
 {
     struct usb_bus *bus;
     struct usb_device *dev;
 
     qDebug ("Scanning for device with VendorId = 0x%04x and ProductId = 0x%04x", VendorId, ProductId);
     usb_find_busses ();
     usb_find_devices ();
 
     for (bus = usb_get_busses(); bus; bus = bus->next)
     {
         for (dev = bus->devices; dev; dev = dev->next)
         {
             if (dev->descriptor.idVendor == VendorId && dev->descriptor.idProduct == ProductId)
             {
                 qDebug ("Device found");
                 if (!(this->m_UsbDevice = usb_open(dev)))
                 {
                     qCritical ("Could not open USB device");
                     return false;
                 }
                 this->m_IsOpened = true;
                 return true;
             }
         }
     }
 
     qDebug ("Cannot find specified device");
     return false;
 }

 bool QUSB::isDevicePresent(quint16 vendorId, quint16 productId) {
     Logger log("bool QUSB::isDevicePresent(quint16 vendorId, quint16 productId)");
     log.stream() << "Looking for Vendor '" << vendorId
             << "', product '" << productId << "'";
     // TODO: WHEN TO USE USB_INIT???
     usb_init();
     struct usb_bus *bus;
     struct usb_device *dev;
     usb_find_busses ();
     usb_find_devices ();

     for (bus = usb_get_busses(); bus; bus = bus->next) {
         for (dev = bus->devices; dev; dev = dev->next) {
             log.stream() << "Checking aginst '" << dev->descriptor.idVendor
                     << "', product '" << dev->descriptor.idProduct << "'";
             if (dev->descriptor.idVendor == vendorId && dev->descriptor.idProduct == productId) {
                 return true;
             }
         }
     }
     return false;
 }
 bool QUSB::isDevicePresent(const Id &id) {
     return isDevicePresent( id.vendorId, id.productId );

 }

 
 bool  QUSB::Open (quint16 VendorId, quint16 ProductId)
 {
     if (this->m_IsOpened) this->Close ();
 
     qDebug ("Initializing LibUSB");
     usb_init ();
     //usb_set_debug (255);
 
     if (!this->FindDevice (VendorId, ProductId)) return false;
 
 #ifndef Q_OS_WIN32
     if (usb_reset (this->m_UsbDevice))
     {
         qCritical ("Cannot reset usb connection.");
         this->Close ();
         return false;
     }
     qDebug ("Resetting usb connection.");
 #endif
 
     if (usb_set_configuration (this->m_UsbDevice, this->CONFIGURATION) < 0)
     {
         qCritical ("Cannot set configuration 1.");
         this->Close ();
         return false;
     }
     qDebug ("Configuration 1 setted");
 
     if (usb_claim_interface (this->m_UsbDevice, this->INTERFACE) < 0)
     {
         qCritical ("Cannot claim interface.");
         this->Close ();
         return false;
     }
     qDebug ("Interface claimed");
 
     return true;
 }
 
 bool QUSB::Write (char *packet, quint8 size)
 {
     if (!this->m_IsOpened)
     {
         qCritical ("Usb connection not opened");
         return false;
     }
 
     if (usb_bulk_write (this->m_UsbDevice, this->ENDPOINT_OUT, packet, (int)size, this->TIMEOUT) < 0)
     {
         qCritical ("Bulk xfer write failed");
         return false;
     }
     return true;
 }
 
 bool QUSB::Read (char *packet, quint8 size)
 {
     if (!this->m_IsOpened)
     {
         qCritical ("Usb connection not opened");
         return false;
     }
 
     if (usb_bulk_read (this->m_UsbDevice, this->ENDPOINT_IN, packet, (int)size, this->TIMEOUT) < 0)
     {
         qCritical ("Bulk xfer read failed");
         return false;
     }
     return true;
 }
 
 void QUSB::Close ()
 {
     qDebug ("Closing LibUSB");
     usb_release_interface (this->m_UsbDevice, this->INTERFACE);
     usb_close (this->m_UsbDevice);
     this->m_IsOpened = false;
 }
} // namespace
