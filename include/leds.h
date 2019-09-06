#ifndef _leds_h
#define _leds_h

#define LED_ACT (1 << 0)
#define LED_RX  (1 << 2)
#define LED_TX  (1 << 3)

#define LED_ON(_x) do { LPC_GPIO3->DATA |= (_x); } while(0)
#define LED_OFF(_x) do { LPC_GPIO3->DATA &= ~(_x); } while(0)
#define LED_TOGGLE(_x) do { LPC_GPIO3->DATA ^= (_x); } while(0)

#endif
