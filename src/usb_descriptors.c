#include "usb_definitions.h"
#include "usb_descriptors.h"

/* Use a very similar descriptor to the ROM driver: section 11.6.1 */

const uint8_t usb_device_descriptor[] = {
  0x12, // bLength, Descriptor size in bytes; update in header if this changes
  0x01, // bDescriptorType, Constant "Device"
  0x00, // bcdUSB LSB of 0x0200 (USB specification release number)
  0x02, // bcdUSB MSB of 0x0200
  0x00, // bDeviceClass Class code
  0x00, // bDeviceSubClass Subclass code
  0x00, // bDeviceProtocol Protocol code
  0x40, // bMaxPacketSize0 Max packet size for endpoint 0 (control)
  0xcc, // idVendor
  0x04, // idVendor Vendor ID 0x04cc
  0x02, // idProduct
  0x08, // idProduct Product ID 0x0802
  0x00, // bcdDevice
  0x01, // bcdDevice Device release 0x0100
  0x01, // iManufacturer string index
  0x02, // iProduct string index
  0x03, // iSerialNumber string index
  0x01  // bNumConfigurations Number of possible configurations
};

/* Use a very similar descriptor to the ROM driver: section 11.6.3 */

const uint8_t usb_configuration_descriptor[] = {
  /* HID configuration descriptor */
  0x09, // bLength Descriptor size in bytes
  0x02, // bDescriptorType Constant "Configuration"
  0x29, // wTotalLength  Size of all data returned for this configuration
  0x00, // wTotalLength  in bytes; update in header if this changes
  0x01, // bNumInterfaces: number of interfaces the configuration supports
  0x01, // bConfigurationValue: identifier for Set/Get_Configuration requests
  0x00, // iConfiguration string index
  0xc0, // bmAttributes
  0x32, // bMaxPower bus power required, expressed as (max mA/2)
  /* HID interface descriptor */
  0x09, // bLength Descriptor size in bytes
  0x04, // bDescriptorType Constant "Interface"
  0x00, // bInterfaceNumber
  0x00, // bAlternateSetting
  0x02, // bNumEndpoints
  0x03, // bInterfaceClass Class code, Human Interface
  0x00, // bInterfaceSubClass, none
  0x00, // bInterfaceProtocol, none
  0x00, // iInterface string index
  /* HID class descriptor */
  0x09, // bLength Descriptor size in bytes
  0x21, // bDescriptorType Constant "HID class"
  0x00, // bcdHID
  0x01, // bcdHID HID specification release number
  0x00, // bCountryCode
  0x01, // bNumDescriptors Number of subordinate class descriptors
  0x22, // bDescriptorType type of class descriptor: HID Report
  0x1b, // wDescriptorLength
  0x00, // wDescriptorLength total length of report descriptor
  /* HID interrupt IN descriptor */
  0x07, // bLength
  0x05, // bDescriptorType "Endpoint"
  0x81, // bEndpointAddress - 1, IN
  0x03, // bmAttributes transfer type supported: interrupt
  0x40, // wMaxPacketSize
  0x00, // wMaxPacketSize max packet size supported
  0x20, // bInterval: polling interval in ms - user configurable
  /* HID interrupt OUT descriptor */
  0x07, // bLength
  0x05, // bDescriptorType "Endpoint"
  0x01, // bEndpointAddress - 1, OUT
  0x03, // bmAttributes transfer type supported: interrupt
  0x40, // wMaxPacketSize
  0x00, // wMaxPacketSize max packet size supported
  0x20  // bInterval: polling interval in ms - user configurable
};

const uint8_t usb_string_languages[] = {
  0x04, // length
  DESCRIPTOR_TYPE_STRING,
  0x09, // 0x0409 = US English
  0x04
};

const uint8_t usb_string_manufacturer[] = {
  14,  // length
  DESCRIPTOR_TYPE_STRING,
  'h',0,
  'a',0,
  's',0,
  's',0,
  'e',0,
  'b',0
};

const uint8_t usb_string_product[] = {
  24,  // length
  DESCRIPTOR_TYPE_STRING,
  'D',0,
  'A',0,
  'L',0,
  'I',0,
  ' ',0,
  'M',0,
  'a',0,
  's',0,
  't',0,
  'e',0,
  'r',0
};

/* HID report descriptor */
const uint8_t usb_hid_report_descriptor[] = {
  // Usage page: generic desktop
  0x06, 0x01, 0x00,
  // Usage: vendor usage 1
  0x09, 0x01,
  // Start of collection Application
  0xa1, 0x01,
  // Logical minimum 0x00
  0x15, 0x00,
  // Logical maximum 0xff
  0x26, 0xff, 0x00,
  // Report size: 8 bits
  0x75, 0x08,
  // Report count: 2 (user configurable, input)
  0x95, 0x02,
  // Usage: vendor usage 1
  0x09, 0x01,
  // Input: data, variable, absolute
  0x81, 0x02,
  // Report count: 2 (user configurable, output)
  0x95, 0x02,
  // Usage: vendor usage 1
  0x09, 0x01,
  // Output: data, variable, absolute
  0x91, 0x02,
  // End collection
  0xc0
};
