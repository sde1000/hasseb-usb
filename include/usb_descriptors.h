#ifndef _usb_descriptors_h
#define _usb_descriptors_h

#include <stdint.h>

extern const uint8_t usb_device_descriptor[0x12];
extern const uint8_t usb_configuration_descriptor[0x29];
extern const uint8_t usb_string_languages[4];
extern const uint8_t usb_string_manufacturer[14];
extern const uint8_t usb_string_product[24];
extern const uint8_t usb_hid_report_descriptor[27];

#endif
