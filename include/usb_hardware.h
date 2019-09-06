/* Definitions for the USB hardware on the LPC1343. */

#ifndef _usb_hardware_h
#define _usb_hardware_h

/* USB endpoints */

/* Logical endpoints assigned as follows:
   0: control
 1-3: interrupt/bulk
   4: isochronous
*/

#define PHYSICAL_ENDPOINT_OUT(_log_endpoint) (_log_endpoint << 1)
#define PHYSICAL_ENDPOINT_IN(_log_endpoint) ((_log_endpoint << 1) | 1)

/* USB hardware registers: manual section 10.10 */

/* USBDevIntSt, USBDevIntEn, USBDevIntClr, USBDevIntSet */
/* NB the device manual refers to the endpoint bits by physical
   endpoint number.  NB logical endpoint 4 can't generate interrupts;
   you would be using INT_FRAME for iso transfers. */
#define INT_FRAME    0x00000001
#define INT_EP0_OUT  0x00000002
#define INT_EP0_IN   0x00000004
#define INT_EP1_OUT  0x00000008
#define INT_EP1_IN   0x00000010
#define INT_EP2_OUT  0x00000020
#define INT_EP2_IN   0x00000040
#define INT_EP3_OUT  0x00000080
#define INT_EP3_IN   0x00000100
#define INT_DEV_STAT 0x00000200
#define INT_CC_EMPTY 0x00000400
#define INT_CD_FULL  0x00000800
#define INT_RxENDPKT 0x00001000
#define INT_TxENDPKT 0x00002000

/* USBCmdCode */
#define SIE_COMMAND(_x) (0x00000500 | (_x << 16))
#define SIE_READ(_x)    (0x00000200 | (_x << 16))
#define SIE_WRITE(_x)   (0x00000100 | (_x << 16))

/* SIE command codes to be used with SIE_COMMAND, SIE_READ */
#define SIE_SetAddress             0xD0 // Write 1 byte
#define SIE_ConfigureDevice        0xD8 // Write 1 byte
#define SIE_SetMode                0xF3 // Write 1 byte
#define SIE_ReadInterruptStatus    0xF4 // Read 1 or 2 bytes
#define SIE_ReadCurrentFrameNumber 0xF5 // Read 1 or 2 bytes
#define SIE_ReadChipID             0xFD // Read 2 bytes
#define SIE_SetDeviceStatus        0xFE // Write 1 byte
#define SIE_GetDeviceStatus        0xFE // Read 1 byte
#define SIE_GetErrorCode           0xFF // Read 1 byte
#define SIE_SelectEndpoint(_x)     (0x00 + _x) // Read 1 byte (optional)
#define SIE_SelectEndpointClearInterrupt(_x) (0x40 + _x) // Read 1 byte
#define SIE_SetEndpointStatus(_x)  (0x40 + _x) // Write 1 byte
#define SIE_ClearBuffer            0xF2 // Read 1 byte (optional)
#define SIE_ValidateBuffer         0xFA // No data

/* Set Address command bits */
#define DEV_EN 0x80 // Device Enable

/* Get/SetDeviceStatus bits */
#define STATUS_CON    0x01
#define STATUS_CON_CH 0x02
#define STATUS_SUS    0x04
#define STATUS_SUS_CH 0x08
#define STATUS_RST    0x10

/* Select Endpoint command status bits */
#define ENDPOINT_FE       0x01 // Full/Empty
#define ENDPOINT_ST       0x02 // Stalled
#define ENDPOINT_STP      0x04 // Received SETUP packet
#define ENDPOINT_PO       0x08 // Packet over-written
#define ENDPOINT_EPN      0x10 // EP NAKed
#define ENDPOINT_B_1_FULL 0x20 // Buffer 1 is full
#define ENDPOINT_B_2_FULL 0x40 // Buffer 2 is full

/* Set Endpoint Status command bits */
#define SET_ENDPOINT_ST     0x01
#define SET_ENDPOINT_DA     0x20
#define SET_ENDPOINT_RF_MO  0x40
#define SET_ENDPOINT_CND_ST 0x80

/* USBRxPLen */
#define DATA_VALID      (1 << 10)

/* USBCtrl */
#define RD_EN           (1 << 0)
#define WR_EN           (1 << 1)
#define LOG_ENDPOINT(x) (x << 2)

#endif
