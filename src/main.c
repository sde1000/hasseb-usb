#include "LPC13xx.h"
#include "leds.h"
#include "iap.h"
#include "usb.h"

static void init_leds(void)
{
  /* Set LED pins to output */
  LPC_GPIO3->DIR |= LED_ACT;
  LPC_GPIO3->DIR |= LED_RX;
  LPC_GPIO3->DIR |= LED_TX;
}

uint32_t get_device_serial(void)
{
  uint32_t uid[4];
  uint32_t serial;
  /* Read the device UID from IAP */
  read_uid(uid);
  /* The serial number will be the four words of the UID XORed together */
  serial = uid[0] ^ uid[1] ^ uid[2] ^ uid[3];
  return serial;
}

int main(void)
{
    SystemInit();
    init_leds();
    LED_OFF(LED_ACT);
    LED_OFF(LED_RX);
    LED_OFF(LED_TX);
    init_usb();
    while(1) {
      /* Enter sleep mode until the next interrupt. */
      
      LPC_PMU->PCON &= ~(1 << 1); // Set DPDEN to zero
      SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk; // Set SLEEPDEEP bit in SCR to zero
      __DSB();
      __WFI();
    }
    return 0;
}
