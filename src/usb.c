#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "LPC13xx.h"
#include "usb.h"
#include "usb_hardware.h"
#include "usb_definitions.h"
#include "usb_descriptors.h"
#include "delay_routines.h"

#include "leds.h"

#define min(a, b) (a < b ? a : b)
#define WEAK_ALIAS(x) __attribute__ ((weak, alias(#x)))

/* Maximum packet sizes are declared in the descriptor */
static uint8_t receivedData[0x40];
static uint8_t current_configuration = 0;

/* Manual sections refer to NXP doc UM10375 rev 5 */
  
/* Basic functions to communicate with the USB hardware */

/* SIE command */
static void sieCommand(uint8_t command) {
  LPC_USB->DevIntClr = INT_CC_EMPTY;
  LPC_USB->CmdCode = SIE_COMMAND(command);
  while(!(LPC_USB->DevIntSt & (INT_CC_EMPTY | INT_DEV_STAT))) {}
}

/* SIE command writing 1 byte of data */
static void writeToSIE(uint8_t command, uint8_t data) {
  sieCommand(command);
  LPC_USB->DevIntClr = INT_CC_EMPTY;
  LPC_USB->CmdCode = SIE_WRITE(data);
  while(!(LPC_USB->DevIntSt & (INT_CC_EMPTY | INT_DEV_STAT))) {}
}

/* SIE command reading 1 byte of data */
static uint8_t readFromSIE(uint8_t command) {
  sieCommand(command);
  LPC_USB->DevIntClr = INT_CD_FULL;
  LPC_USB->CmdCode = SIE_READ(command);
  while(!(LPC_USB->DevIntSt & (INT_CD_FULL | INT_DEV_STAT))) {}
  return LPC_USB->CmdData;
}

/* SIE commands */

//static uint8_t getSIEerrorCode(void) {
//  return readFromSIE(SIE_GetErrorCode) & 0xF;
//}

/* These three functions that operate on endpoints require the
   PHYSICAL endpoint number, i.e. specify PHYSICAL_ENDPOINT_IN(number)
   or PHYSICAL_ENDPOINT_OUT(number) */
static uint8_t selectEndpoint(uint8_t physicalEndpointIndex) {
  return readFromSIE(SIE_SelectEndpoint(physicalEndpointIndex));
}

static uint8_t selectEndpointClearInterrupt(uint8_t physicalEndpointIndex) {
  uint8_t endpointInformation =
    readFromSIE(SIE_SelectEndpointClearInterrupt(physicalEndpointIndex));
  return endpointInformation;
}

static void setEndpointStatus(uint8_t physicalEndpointIndex, uint8_t status) {
  writeToSIE(SIE_SetEndpointStatus(physicalEndpointIndex), status);
}

/* This operates on the currently selected endpoint? */
static bool clearBuffer(void) {
  return readFromSIE(SIE_ClearBuffer) & 0x01;
}

/* This operates on the currently selected endpoint? */
static void validateBuffer(void) {
  sieCommand(SIE_ValidateBuffer);
}

static void setConfigured(bool configured) {
  writeToSIE(SIE_ConfigureDevice, configured ? 0x01 : 0x00);
}


/* Reading data in the Out direction */
static uint32_t readDataFromEndpoint(uint8_t logicalEndpoint) {
  uint32_t counter = 0;
  uint32_t RxPLen;
  uint32_t rxData;
  uint16_t length;
  //bool valid;

  memset(receivedData, 0, 64 * sizeof(uint8_t));

  LPC_USB->Ctrl = (RD_EN | LOG_ENDPOINT(logicalEndpoint));
  __NOP();
  __NOP();
  __NOP();
  /* Experiment: loop until DV is set */
  do {
    RxPLen = LPC_USB->RxPLen;
  } while ((RxPLen & DATA_VALID) == 0);
  length = RxPLen & 0x3ff;
  //valid = RxPLen & DATA_VALID;

  uint32_t numberOfDwordsToRead = ((length + 3) >> 2);
  while(counter < numberOfDwordsToRead) {
    rxData = LPC_USB->RxData;
    receivedData[counter*4]   = (rxData & 0x000000FF);
    receivedData[counter*4+1] = ((rxData & 0x0000FF00) >> 8);
    receivedData[counter*4+2] = ((rxData & 0x00FF0000) >> 16);
    receivedData[counter*4+3] = ((rxData & 0xFF000000) >> 24);
    counter += 1;
  }
  if (length == 0) {
    rxData = LPC_USB->RxData;
  }

  LPC_USB->Ctrl = 0;
  selectEndpoint(PHYSICAL_ENDPOINT_OUT(logicalEndpoint));
  clearBuffer();

  /* Do anything with the "valid" flag? */
  return length;
}

/* Writing data in the IN direction */
static void sendDataToEndpoint(uint8_t logicalEndpoint, const uint8_t* buffer, uint8_t length) {
  uint8_t counter = 0;

  LPC_USB->DevIntClr = INT_TxENDPKT;
  LPC_USB->Ctrl = (WR_EN | LOG_ENDPOINT(logicalEndpoint));
  __NOP();
  __NOP();
  __NOP();
  LPC_USB->TxPLen = (length & 0x3FF);

  while (counter < length) {
    uint32_t dwordToSend = 0;
    dwordToSend |= buffer[counter];
    dwordToSend |= (buffer[counter+1] << 8);
    dwordToSend |= (buffer[counter+2] << 16);
    dwordToSend |= (buffer[counter+3] << 24);
    LPC_USB->TxData = dwordToSend;
    counter += 4;
  }

  if (length == 0) {
    LPC_USB->TxData = 0x0;
  }

  LPC_USB->Ctrl = 0;
  selectEndpoint(PHYSICAL_ENDPOINT_IN(logicalEndpoint));
  validateBuffer();
}

static void setupError(void) {
  /* If we need to respond to a SETUP packet with "error" then we send
     a stall. */
  setEndpointStatus(PHYSICAL_ENDPOINT_IN(0), SET_ENDPOINT_ST);
}

static uint8_t hexdigits[] = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

static void sendDescriptor(uint8_t type, uint8_t index, uint16_t length) {
  uint8_t serial_descriptor[18];
  uint8_t i;
  uint32_t serial;
  switch (type) {
  case DESCRIPTOR_TYPE_DEVICE:
    sendDataToEndpoint(CONTROL_ENDPOINT, usb_device_descriptor,
		       min(length, sizeof(usb_device_descriptor)));
    break;
  case DESCRIPTOR_TYPE_CONFIGURATION:
    sendDataToEndpoint(CONTROL_ENDPOINT, usb_configuration_descriptor,
		       min(length, sizeof(usb_configuration_descriptor)));
    break;
  case DESCRIPTOR_TYPE_STRING:
    switch (index) {
    case 0:
      /* Languages descriptor */
      sendDataToEndpoint(CONTROL_ENDPOINT, usb_string_languages,
			 min(length, sizeof(usb_string_languages)));
      break;
    case 1:
      /* Manufacturer */
      sendDataToEndpoint(CONTROL_ENDPOINT, usb_string_manufacturer,
			 min(length, sizeof(usb_string_manufacturer)));
      break;
    case 2:
      /* Product */
      sendDataToEndpoint(CONTROL_ENDPOINT, usb_string_product,
			 min(length, sizeof(usb_string_product)));
      break;
    case 3:
      /* Serial */
      serial = get_device_serial();
      memset(serial_descriptor, 0, sizeof(serial_descriptor));
      serial_descriptor[0] = 18;
      serial_descriptor[1] = DESCRIPTOR_TYPE_STRING;
      for (i=0; i<8; i++) {
	uint8_t nibble;
	nibble = (serial & 0xf0000000) >> 28;
	serial = serial << 4;
	serial_descriptor[2 + (i * 2)] = hexdigits[nibble];
      }
      sendDataToEndpoint(CONTROL_ENDPOINT, serial_descriptor,
			 min(length, sizeof(serial_descriptor)));
      break;
    default:
      setupError();
      break;
    }
    break;
  default:
    setupError();
    break;
  }
}

static void handleSetup(void) {
  uint8_t bmRequestType = 0;
  uint8_t request = 0;
  uint16_t value = 0;
  uint16_t index = 0;
  uint16_t length = 0;
  uint8_t status[] = {0, 0};

  uint8_t *data = receivedData;

  readDataFromEndpoint(CONTROL_ENDPOINT);

  bmRequestType = data[0];
  request = data[1];
  value = data[2];
  value |= (data[3] << 8);
  index = data[4];
  index |= (data[5] << 8);
  length = data[6];
  length |= (data[7] << 8);

  if (SETUP_RECIPIENT(bmRequestType) == SETUP_RECIPIENT_DEVICE) {
    switch (request) {
    case GET_DESCRIPTOR:
      sendDescriptor(data[3], data[2], length);
      break;

    case SET_ADDRESS:
      /* Should not change the device address until AFTER the status stage */
      sendDataToEndpoint(CONTROL_ENDPOINT, NULL, 0);
      writeToSIE(SIE_SetAddress, value | DEV_EN);
      writeToSIE(SIE_SetAddress, value | DEV_EN);
      break;

    case SET_CONFIGURATION:
      current_configuration = value & 0xff;
      sendDataToEndpoint(CONTROL_ENDPOINT, NULL, 0);
      if (current_configuration == 1) {
	LED_ON(LED_TX);
	setConfigured(true);
	setEndpointStatus(PHYSICAL_ENDPOINT_IN(1), 0);
	setEndpointStatus(PHYSICAL_ENDPOINT_OUT(1), 0);
	selectEndpoint(PHYSICAL_ENDPOINT_OUT(1));
	clearBuffer();
      } else {
	LED_OFF(LED_TX);
	setConfigured(false);
      }
      break;

    case GET_CONFIGURATION:
      sendDataToEndpoint(CONTROL_ENDPOINT, &current_configuration, 1);
      break;

    case GET_STATUS:
      /* Our status is always "zero" */
      sendDataToEndpoint(CONTROL_ENDPOINT, status, 2);
      break;

    default:
      /* Unknown request - stall */
      setupError();
      break;
    }
  } else if (SETUP_RECIPIENT(bmRequestType) == SETUP_RECIPIENT_INTERFACE) {
    switch (request) {
    case HID_REQUEST_SET_IDLE:
      sendDataToEndpoint(CONTROL_ENDPOINT, NULL, 0);
      break;
    case GET_DESCRIPTOR:
      sendDataToEndpoint(CONTROL_ENDPOINT, usb_hid_report_descriptor,
			 min(length, sizeof(usb_hid_report_descriptor)));
      break;
    default:
      setupError();
    }
  } else {
    setupError();
  }
}

static void usb_reset(void) {
  current_configuration = 0;
}

void USB_IRQ_Handler(void) {
  uint32_t interrupts;
  uint8_t endpointInfo;

  LED_ON(LED_ACT);
  interrupts = LPC_USB->DevIntSt;
  LPC_USB->DevIntClr = interrupts;

  if (interrupts & INT_DEV_STAT) {
    uint8_t status = readFromSIE(SIE_GetDeviceStatus);
    if (status & STATUS_RST) {
      usb_reset();
    }
  }

  if (interrupts & INT_EP0_OUT) {
    endpointInfo =
      selectEndpointClearInterrupt(PHYSICAL_ENDPOINT_OUT(0));
    if (endpointInfo & ENDPOINT_STP) {
      handleSetup();
    } else if (endpointInfo & ENDPOINT_FE) {
      readDataFromEndpoint(0);
    }
  }

  if (interrupts & INT_EP0_IN) {
    /* Ack any outgoing data */
    endpointInfo =
      selectEndpointClearInterrupt(PHYSICAL_ENDPOINT_IN(0));
  }

  if (interrupts & INT_EP1_OUT) {
    /* Check for data for us */
    endpointInfo = selectEndpointClearInterrupt(PHYSICAL_ENDPOINT_OUT(1));
    if (endpointInfo & ENDPOINT_FE) {
      uint32_t length;
      length = readDataFromEndpoint(1);
      if (length > 0) {
	if (receivedData[0] == '0') {
	  LED_ON(LED_RX);
	} else {
	  LED_OFF(LED_RX);
	}
      }
      sendDataToEndpoint(1, receivedData, length);
    }
  }

  if (interrupts & INT_EP1_IN) {
    /* Ack any outgoing data */
    selectEndpointClearInterrupt(PHYSICAL_ENDPOINT_IN(1));
  }
  LED_OFF(LED_ACT);
}

uint32_t dummy_device_serial(void) {
  return 0x12345678;
}

/* Override this in another module to set the serial number */
uint32_t get_device_serial(void) WEAK_ALIAS(dummy_device_serial);

void init_usb(void) {
  NVIC_DisableIRQ(USB_IRQn);
  
  /* USB clock init is done in SystemInit in system_LPC13xx.c */

  LPC_SYSCON->USBCLKUEN = 0x01;
  LPC_SYSCON->USBCLKUEN = 0x00;
  LPC_SYSCON->USBCLKUEN = 0x01;
  LPC_SYSCON->USBCLKDIV = 0x01;
  
  /* Section 10.12.2.1 - enable USB and IOConfig blocks */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 16);
  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);
  /* Do we also need GPIO? */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);
  /* Do we also need TMR32_1? */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 10);
  /* Section 10.12.2.2 */
  LPC_IOCON->PIO0_3 = 0xd0; // Function USB_VBUS not used - pin not connected
  LPC_IOCON->PIO0_6 = 0xd5; // Function USB_CONNECT - but does it work?

  /* Section 10.2.2 - power */
  LPC_SYSCON->PDRUNCFG &= ~(1 << 10);

  uint8_t counter = 75;
  while(counter) {
    __NOP();
    counter--;
  }

  //writeToSIE(SIE_SetMode, 0x01); // Keep clock running through suspend

  writeToSIE(SIE_SetAddress, 0 | DEV_EN);
  writeToSIE(SIE_SetAddress, 0 | DEV_EN);
  setEndpointStatus(PHYSICAL_ENDPOINT_OUT(0), 0);
  setEndpointStatus(PHYSICAL_ENDPOINT_IN(0), 0);
  selectEndpoint(PHYSICAL_ENDPOINT_OUT(0));
  clearBuffer();

  /* Section 10.12.2.3 - interrupts */
  NVIC_ClearPendingIRQ(USB_IRQn);
  LPC_USB->DevIntClr = 0x0003fff;
  LPC_USB->DevIntEn = INT_DEV_STAT | INT_EP0_OUT | INT_EP0_IN | INT_EP1_OUT | INT_EP1_IN;
  writeToSIE(SIE_SetDeviceStatus, STATUS_CON); // Set CON bit

  NVIC_EnableIRQ(USB_IRQn);
}
