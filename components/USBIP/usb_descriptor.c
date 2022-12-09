 ////TODO: refactoring into structure
/**
 * @file usb_descriptor.c
 * @brief Standard USB Descriptor Definitions
 * @change: 2020-1-23 : fix bugs
 *          2021-5-12 : Add support for USB 3.0
 * @version 0.2
 * @date 2020-1-23
 *
 *
 */
#include <stdint.h>
#include <stdbool.h>

#include "components/USBIP/usb_descriptor.h"

#define USBShort(ui16Value)     ((ui16Value) & 0xff), ((ui16Value) >> 8)


/**
 * @brief step 1. Build Standard Device Descriptor
 *
 */

// Standard Device Descriptor
const uint8_t kUSBd0DeviceDescriptor[0x12] =
{
    0x12,             // bLength
    USB_DT_DEVICE,    // bDescriptorType

#if (USE_WINUSB == 1)

#if (USE_USB_3_0 == 1)
    USBShort(0x0300), // bcdUSB
#else
    USBShort(0x210),  // bcdUSB
#endif // USE_USB_3_0 == 1

#else
    USBShort(0x0200), // bcdUSB
#endif // (USE_WINUSB == 1)
    ////TODO: Is it also available elsewhere?

    // We need to use a device other than the USB-IF standard, set to 0x00
    0x00, // bDeviceClass
    0x00, // bDeviceSubClass
    0x00, // bDeviceProtocol

#if (USE_USB_3_0 == 1)
    0x09,                               // bMaxPacketSize0, for USB 3.0 must set to 0x09(2^9)
#else
    USBD0_MAX_PACKET0,                  // bMaxPacketSize0 Maximum packet size for default pipe.
#endif
    USBShort(USBD0_DEV_DESC_IDVENDOR),  // idVendor Vendor ID (VID).
    USBShort(USBD0_DEV_DESC_IDPRODUCT), // idProduct Product ID (PID).
    USBShort(USBD0_DEV_DESC_BCDDEVICE), // bcdDevice Device Version BCD.
    0x01,                               // iManufacturer Index of Manufacturer string identifier.
    0x02,                               // iProduct Index of Product string identifier.
    0x03 * USBD0_STR_DESC_SER_EN,       // iSerialNumber Index of Product serial number.
    0x01                                // bNumConfigurations Number of configurations.
};



/**
 * @brief step 2. Buid Standard Configuration Descriptor
 *
 */

// Standard Configuration Descriptor
#define LENGTHOFCONFIGDESCRIPTOR 9

const uint8_t kUSBd0ConfigurationDescriptor[] =
{
// const uint8_t kUSBd0ConfigDescriptor[LENGTHOFCONFIGDESCRIPTOR] =
#if (USE_WINUSB == 1)
    // Configuration descriptor header.

    0x09,                                   // bLength
    USB_DT_CONFIGURATION,                   // bDescriptorType

    USBShort(sizeof(kUSBd0ConfigurationDescriptor)),
                                            // wTotalLength

    0x01,                                   // bNumInterfaces
                                            // There is only one interface in the CMSIS-DAP project
    0x01,                                   // bConfigurationValue: 0x01 is used to select this configuration */
    0x00,                                   // iConfiguration: no string to describe this configuration */
    USBD0_CFG_DESC_BMATTRIBUTES,            // bmAttributes

    USBD0_CFG_DESC_BMAXPOWER,               // bMaxPower

#else
    // Configuration descriptor header.

    0x09,                                   // bLength
    USB_DT_CONFIGURATION,                   // bDescriptorType

    USBShort(sizeof(kUSBd0ConfigurationDescriptor)),
                                            // wTotalLength

    0x01,                                   // bNumInterfaces
                                            // There is only one interface in the CMSIS-DAP project
    0x01,                                   // bConfigurationValue: 0x01 is used to select this configuration */
    0x00,                                   // iConfiguration: no string to describe this configuration */
    USBD0_CFG_DESC_BMATTRIBUTES,            // bmAttributes

    USBD0_CFG_DESC_BMAXPOWER,               // bMaxPower
#endif

// Standard Interface Descriptor

// const uint8_t kUSBd0InterfaceDescriptor[]=
#if (USE_WINUSB ==1)
    0x09,                                   // bLength
    USB_DT_INTERFACE,                       // bDescriptorType
    USBD_CUSTOM_CLASS0_IF0_NUM,             // bInterfaceNumber
    USBD_CUSTOM_CLASS0_IF0_ALT,             // bAlternateSetting
    0x03,                                   // bNumEndpoints(we will use 3 endpoints)
                                            //
    USBD_CUSTOM_CLASS0_IF0_CLASS,           // bInterfaceClass
    USBD_CUSTOM_CLASS0_IF0_SUBCLASS,        // bInterfaceSubClass
    USBD_CUSTOM_CLASS0_IF0_PROTOCOL,        // bInterfaceProtocol
    0x04,                                   // iInterface
                                            // Index of string descriptor describing this interface
    ////TODO: fix this 0x04 ?


    // Standard Endpoint Descriptor

    // Endpoint 1: Bulk Out – used for commands received from host PC.
    // Endpoint 2: Bulk In – used for responses send to host PC.
    // Endpoint 3: Bulk In (optional) – used for streaming SWO trace

    // ATTENTION:
    // physical endpoint 1 indeed included two "endpoints": Bulk OUT and Bulk IN
    // physical endpoint 1 -> Endpoint 1 & Endpoint 2
    // physical endpoint 2 -> Endpoint 3

    // See also :
    // http://www.keil.com/pack/doc/CMSIS/DAP/html/group__DAP__ConfigUSB__gr.html

    /*                 Pysical endpoint 1                 */

    // "Endpoint 1: Bulk Out – used for commands received from host PC."  PC -> Device
    0x07,                                                      // bLength
    USB_DT_ENDPOINT,                                           // bDescriptorType
    0x01,                                                      // bEndpointAddress
    USB_ENDPOINT_ATTR_BULK,                                    // bmAttributes
    USBShort(USB_ENDPOINT_SIZE),                               // wMaxPacketSize
    0x00, // bInterval

    /*                 SuperSpeed Endpoint Companion      */
#if (USE_USB_3_0 == 1)
    0x06,                                                      // bLength
    USB_DT_SUPERSPEED_USB_ENDPOINT_COMPANION,                  // bDescriptorType
    0x00,                                                      // bMaxBurst
    0x00,                                                      // bmAttributes(MaxStream for Bulk)
    0x00, 0x00,                                                // wBytesPerInterval -> 0 for Bulk
#endif // USE_USB_3_0 == 1


    /*                 Pysical endpoint 1                 */

    // "Endpoint 2: Bulk In – used for responses send to host PC." Device -> PC
    0x07,                                                      // bLength
    USB_DT_ENDPOINT,                                           // bDescriptorType
    0x81,                                                      // bEndpointAddress
    USB_ENDPOINT_ATTR_BULK,                                    // bmAttributes
    USBShort(USB_ENDPOINT_SIZE),                               // wMaxPacketSize
    0x00,                                                      // bInterval

    /*                 SuperSpeed Endpoint Companion      */
#if (USE_USB_3_0 == 1)
    0x06,                                                      // bLength
    USB_DT_SUPERSPEED_USB_ENDPOINT_COMPANION,                  // bDescriptorType
    0x00,                                                      // bMaxBurst
    0x00,                                                      // bmAttributes(MaxStream for Bulk)
    0x00, 0x00,                                                // wBytesPerInterval -> 0 for Bulk
#endif // USE_USB_3_0 == 1


    /*                 Pysical endpoint 2                */

    // "Endpoint 3: Bulk In (optional) – used for streaming SWO trace" Device -> PC
    0x07,                                                      // bLength
    USB_DT_ENDPOINT,                                           // bDescriptorType
    0x82,                                                      // bEndpointAddress
    USB_ENDPOINT_ATTR_BULK,                                    // bmAttributes
    USBShort(USB_ENDPOINT_SIZE),                               // wMaxPacketSize
    0x00,                                                      // bInterval

    /*                 SuperSpeed Endpoint Companion      */
#if (USE_USB_3_0 == 1)
    0x06,                                                      // bLength
    USB_DT_SUPERSPEED_USB_ENDPOINT_COMPANION,                  // bDescriptorType
    0x00,                                                      // bMaxBurst
    0x00,                                                      // bmAttributes(MaxStream for Bulk)
    0x00, 0x00,                                                // wBytesPerInterval -> 0 for Bulk
#endif // USE_USB_3_0 == 1

#else
    0x09,                                   // bLength
    USB_DT_INTERFACE,                       // bDescriptorType
    USBD_CUSTOM_CLASS0_IF0_NUM,             // bInterfaceNumber
    USBD_CUSTOM_CLASS0_IF0_ALT,             // bAlternateSetting
    0x02,                                   // bNumEndpoints ----> 2 endpoint for USB HID
                                            //
    USBD_CUSTOM_CLASS0_IF0_CLASS,           // bInterfaceClass
    USBD_CUSTOM_CLASS0_IF0_SUBCLASS,        // bInterfaceSubClass
    USBD_CUSTOM_CLASS0_IF0_PROTOCOL,        // bInterfaceProtocol
    0x04,                                   // iInterface
                                            // Index of string descriptor describing this interface

    // HID Descriptor
    0x09,                                   // bLength
    0x21,                                   // bDescriptorType
    0x11, 0x01,                             // bcdHID
    0x00,                                   // bCountryCode
    0x01,                                   // bNumDescriptors
    0x22,                                   // bDescriptorType1
    0x21, 0x00,                             // wDescriptorLength1

    // Standard Endpoint Descriptor

    // We perform all transfer operations on Pysical endpoint 1.

    /*                 Pysical endpoint 1                 */

    0x07,                                                      // bLength
    USB_DT_ENDPOINT,                                           // bDescriptorType
    0x81,                                                      // bEndpointAddress
    USB_ENDPOINT_ATTR_INTERRUPT,                               // bmAttributes
    USBShort(64),                                              // wMaxPacketSize
    0x01,                                                      // bInterval

    /*                 Pysical endpoint 1                 */

    0x07,                                                      // bLength
    USB_DT_ENDPOINT,                                           // bDescriptorType
    0x01,                                                      // bEndpointAddress
    USB_ENDPOINT_ATTR_INTERRUPT,                               // bmAttributes
    USBShort(64),                                             // wMaxPacketSize
    0x01,                                                      // bInterval
#endif

#ifdef USE_USB_VCP
    // Interface Association Descriptor
    0x08,       // bLength
    USB_DT_IAD, // bDescriptorType
    USBD_VCP_INT_IF_NUM, // bFirstInterface
    2, // bInterfaceCount
    0x02, // bFunctionClass: Communications and CDC Control (0x02)
    0x02, // bFunctionSubClass: Abstract (modem)
    0x1, // bFunctionProtocol: AT-commands (v.25ter)
    0x5, // iFunction

    // intf 1: CDC interrupt
    0x09,                                   // bLength
    USB_DT_INTERFACE,                       // bDescriptorType
    USBD_VCP_INT_IF_NUM,             // bInterfaceNumber
    0,             // bAlternateSetting
    0x01,                                   // bNumEndpoints
    0x2, // bInterfaceClass         2 Communications
    0x2, // bInterfaceSubClass      2 Abstract (modem)
    0x1, // bInterfaceProtocol      1 AT-commands (v.25ter)
    0x05,                                   // iInterface

    // COMMUNICATIONS DESCRIPTOR
    0x05, // bLength: 5
    0x24, // bDescriptorType: 0x24 (CS_INTERFACE)
    0x00, // Descriptor Subtype: Header Functional Descriptor (0x00)
    0x10, 0x01, // CDC: 0x0110

    // COMMUNICATIONS DESCRIPTOR
    0x5, // bLength: 5
    0x24, // bDescriptorType: 0x24 (CS_INTERFACE)
    0x1, // Descriptor Subtype: Call Management Functional Descriptor (0x01)
    0x03, // bmCapabilities: 0x03
    USBD_VCP_BULK_IF_NUM, // Data Interface:


    // COMMUNICATIONS DESCRIPTOR
    0x4, // bLength: 4
    0x24, // bDescriptorType: 0x24 (CS_INTERFACE)
    0x02, // Descriptor Subtype: Abstract Control Management Functional Descriptor (0x02)
    0x06, // bmCapabilities: 0x06 (sends break, line coding and serial state)

    // COMMUNICATIONS DESCRIPTOR
    0x5, // bLength: 5
    0x24, // bDescriptorType: 0x24 (CS_INTERFACE)
    0x06, // Descriptor Subtype: Union Functional Descriptor (0x06)
    USBD_VCP_INT_IF_NUM, // Control Interface: 0x00
    USBD_VCP_BULK_IF_NUM, // Subordinate Interface: 0x01

    // EP3: CDC interrupt IN
    0x07, // bLength
    USB_DT_ENDPOINT, // bDescriptorType
    (0x80 | USBD_VCP_INT_EP_NUM), // bEndpointAddress
    USB_ENDPOINT_ATTR_INTERRUPT, // bmAttributes
    USBShort(USBD_VCP_EP_SIZE), // wMaxPacketSize
    0x20, // bInterval

    // intf 2: CDC bulk
    0x09,                                   // bLength
    USB_DT_INTERFACE,                       // bDescriptorType
    USBD_VCP_BULK_IF_NUM,             // bInterfaceNumber
    0,             // bAlternateSetting
    0x02,                                   // bNumEndpoints
    0x0a, // bInterfaceClass: CDC-Data (0x0a)
    0x0, // bInterfaceSubClass: 0x00
    0x00, // bInterfaceProtocol: No class specific protocol required (0x00)
    0x5, // iInterface: 5

    // ENDPOINT DESCRIPTOR
    0x7, // bLength: 7
    0x5, // bDescriptorType: 0x05 (ENDPOINT)
    USBD_VCP_BULK_EP_NUM, // bEndpointAddress: 0x04  OUT  Endpoint:4
    USB_ENDPOINT_ATTR_BULK, // bmAttributes
    USBShort(USBD_VCP_EP_SIZE), // wMaxPacketSize: 16
    0x0, // bInterval: 0

    // ENDPOINT DESCRIPTOR
    0x7, // bLength: 7
    0x5, // bDescriptorType: 0x05 (ENDPOINT)
    (0x80 | USBD_VCP_BULK_EP_NUM), // bEndpointAddress: 0x84  IN  Endpoint:4
    USB_ENDPOINT_ATTR_BULK, // bmAttributes
    USBShort(USBD_VCP_EP_SIZE), // wMaxPacketSize: 16
    0x0, // bInterval: 0

#endif // USE_USB_VCP
};



// USB HID Report Descriptor
const uint8_t kHidReportDescriptor[0x21] = {
        0x06, 0x00, 0xFF,  // Usage Page (Vendor Defined 0xFF00)
        0x09, 0x01,        // Usage (0x01)
        0xA1, 0x01,        // Collection (Application)
        0x15, 0x00,        //   Logical Minimum (0)
        0x26, 0xFF, 0x00,  //   Logical Maximum (255)
        0x75, 0x08,        //   Report Size (8)
        0x95, 0xFF,        //   Report Count (64)
        0x09, 0x01,        //   Usage (0x01)
        0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x95, 0xFF,        //   Report Count (64)
        0x09, 0x01,        //   Usage (0x01)
        0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
        0x95, 0x01,        //   Report Count (1)
        0x09, 0x01,        //   Usage (0x01)
        0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
        0xC0,              // End Collection
        // 33 bytes
};






/**
 * @brief step 3. Build String Descriptor
 *
 */


const uint8_t kLangDescriptor[0x04] =
{
    4,
    USB_DT_STRING,
    USBShort(USB_LANGID_ENGLISH_US)
};


/**
 * @brief We will use these string descriptor:
 *        1. Manufacturer string    -> "KEIL - Tools By ARM"
 *        2. Product string         -> "LPC-Link-II"
 *        3. Serial number string   -> "0001A0000000"
 *        4. Interface string       -> "LPC-Link-II CMSIS-DAP"
 *
 *
 */


// -1 byte terminator, to utf-16, +2 byte header
#define STR_DESC_LENGTH(x) (sizeof(x) * 2)

static const uint8_t kManufacturerString[] =
{
    STR_DESC_LENGTH("windowsair"),
    0x03, // bDescriptorType
    'w', 0, 'i', 0, 'n', 0, 'd', 0, 'o', 0, 'w', 0, 's', 0, 'a', 0, 'i', 0,
    'r', 0,
};

static const uint8_t kProductString[] =
{
    STR_DESC_LENGTH("Wireless ESP CMSIS-DAP"),
    0x03, // bDescriptorType
    'W', 0, 'i', 0, 'r', 0, 'e', 0, 'l', 0, 'e', 0, 's', 0, 's', 0, ' ', 0,
    'E', 0, 'S', 0, 'P', 0, ' ', 0, 'C', 0, 'M', 0, 'S', 0, 'I', 0, 'S', 0,
    '-', 0, 'D', 0, 'A', 0, 'P', 0,
};

static const uint8_t kSerialNumberString[] =
{
    STR_DESC_LENGTH("1234"),
    0x03, // bDescriptorType
    '1', 0, '2', 0, '3', 0, '4', 0,
};

static const uint8_t kInterfaceString[] =
{
    STR_DESC_LENGTH("CMSIS-DAP v2"),
    0x03, // bDescriptorType
    'C', 0, 'M', 0, 'S', 0, 'I', 0, 'S', 0, '-', 0, 'D', 0, 'A', 0, 'P', 0,
    ' ', 0, 'v', 0, '2', 0,
};

// No. 5
static const uint8_t kVCPInterfaceString[] =
{
    STR_DESC_LENGTH("CMSIS-DAP CDC"),
    0x03, // bDescriptorType
    'C', 0, 'M', 0, 'S', 0, 'I', 0, 'S', 0, '-', 0, 'D', 0, 'A', 0, 'P', 0,
    ' ', 0, 'C', 0, 'D', 0, 'C', 0,
};

const uint8_t * const kUSBd0StringDescriptorsSet[] =
{
    kLangDescriptor,
    kManufacturerString,
    kProductString,
    kSerialNumberString,
    kInterfaceString,
    kVCPInterfaceString
};


// TODO: empty usb_device_qualifier_descriptor
const usb_device_qualifier_descriptor kUSBd0DeviceQualifierDescriptor = {0};
