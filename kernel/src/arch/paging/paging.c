#include "paging.h"

uint64_t PHYSICAL_ADDRESS_OFFSET = 0;

void init_paging(uint64_t offset) {
  PHYSICAL_ADDRESS_OFFSET = offset;
}
