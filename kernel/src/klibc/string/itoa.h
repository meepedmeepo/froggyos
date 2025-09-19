#ifndef _K_ITOA_H
#define _K_ITOA_H
#include <stdint.h>

char* itoa(int64_t value, char* str, uint8_t base); 
char* utoa(uint64_t value, char* str, uint8_t);

#endif
