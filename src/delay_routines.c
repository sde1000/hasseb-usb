#include <stdint.h>
#include "LPC13xx.h"
#include "system_LPC13xx.h"

/* Delay using CTR16_0 */

void delayMicroseconds(uint32_t count) {
  uint8_t CYCLES_IN_MICROSECOND = (SystemCoreClock / 1000000);

  // enable clocking
  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);

  // reset TC and PC
  LPC_TMR16B0->TCR = (1 << 1);

  // Set prescaler and match registers
  LPC_TMR16B0->PR = CYCLES_IN_MICROSECOND;
  LPC_TMR16B0->MR0 = count;
  // Stop on MR0
  LPC_TMR16B0->MCR |= (1 << 2);
  // enable counter
  LPC_TMR16B0->TCR = 1;

  // wait
  while (LPC_TMR16B0->TCR != 0) {}

  LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 7);
}

void delayMilliseconds(uint16_t millis) {
  uint16_t count = millis;
  while(count) {
    delayMicroseconds(1000);
    count--;
  }
}
