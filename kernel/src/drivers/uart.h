#ifndef _KERNEL_UART_H
#define _KERNEL_UART_H
#include <stdint.h>

#define PORT 0x3f8

void init_serial();

int serial_recieved();

char read_serial();

int uart_transmit_ready();

void write_serial(char a);

void write_serial_string(const char *a);

static inline void outb(uint16_t port, uint8_t val) {
  __asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile ("inb %w1, %b0"
                    : "=a"(ret)
                    : "Nd"(port)
                    : "memory");
  return ret; 
}

#endif
