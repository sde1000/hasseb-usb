/* In Application Programming access */

#include <stdint.h>
#include "iap.h"

#define IAP_LOCATION 0x1fff1ff1

typedef void (*IAP)(uint32_t[], uint32_t[]);

static IAP iap_entry=(IAP)IAP_LOCATION;

void read_uid(uint32_t uid[4]) {
  uint32_t command[1];
  uint32_t result[5];

  command[0] = 58;
  iap_entry(command, result);

  uid[0] = result[1];
  uid[1] = result[2];
  uid[2] = result[3];
  uid[3] = result[4];
}
