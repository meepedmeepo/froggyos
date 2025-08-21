#include "uart.h"

void init_serial() {
  outb(PORT + 1, 0x00);//disable interrupts
  outb(PORT + 3, 0x80);//enable dlab (baudrate divisor)
  outb(PORT + 0, 0x03);//sets divisor to 3 (38400 baud)
  outb(PORT + 1, 0x00);
  outb(PORT + 3, 0x03);//8bit message length with no parity bit, and one stop bit
  outb(PORT + 2, 0xc7);//Enables FIFO with 14bit clear threshold
  outb(PORT + 4 , 0x0B);//interrupts set

  
}

int uart_transmit_ready() {
  return inb(PORT +5) & 1;
}

void write_serial(char a) {
  while (uart_transmit_ready() == 0);

  outb(PORT, a);
}

void write_serial_string(const char *str) {
  char c;

  while ((c = *str++)){
    write_serial(c);  
  }  
}
