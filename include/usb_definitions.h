#ifndef _usb_definitions_h
#define _usb_definitions_h

/* USB Setup requests */

#define SETUP_TYPE(_bmRequestType) ((_bmRequestType >> 5) & 3)
#define SETUP_TYPE_STANDARD 0
#define SETUP_TYPE_CLASS    1
#define SETUP_TYPE_VENDOR   2
#define SETUP_TYPE_RESERVED 3

#define SETUP_RECIPIENT(_bmRequestType) (_bmRequestType & 0x1f)
#define SETUP_RECIPIENT_DEVICE    0
#define SETUP_RECIPIENT_INTERFACE 1
#define SETUP_RECIPIENT_ENDPOINT  2
#define SETUP_RECIPIENT_OTHER     3

#define SETUP_DATA_DIRECTION(_bmRequestType) ((_bmRequestType) >> 7) & 1)

/* Standard setup requests */
#define CLEAR_FEATURE      1
#define GET_CONFIGURATION  8
#define GET_DESCRIPTOR     6
#define GET_INTERFACE      10
#define GET_STATUS         0
#define SET_ADDRESS        5
#define SET_CONFIGURATION  9
#define SET_DESCRIPTOR     7
#define SET_FEATURE        3
#define SET_INTERFACE      11
#define SYNCH_FRAME        12

/* HID setup requests */
#define HID_REQUEST_SET_IDLE   0x0a
#define HID_REQUEST_HID_REPORT 0x22

/* Descriptor types */
#define DESCRIPTOR_TYPE_DEVICE           1
#define DESCRIPTOR_TYPE_CONFIGURATION    2
#define DESCRIPTOR_TYPE_STRING           3
#define DESCRIPTOR_TYPE_INTERFACE        4
#define DESCRIPTOR_TYPE_ENDPOINT         5
#define DESCRIPTOR_TYPE_DEVICE_QUALIFIER 6

#define CONTROL_ENDPOINT        0x0

#endif
