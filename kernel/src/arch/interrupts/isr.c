#include "isr.h"
#include "../../drivers//uart.h"

void isr_exception_handler(isr_stack_frame_t *frame) {
  write_serial_string("Exception Occured! \n");

  asm("hlt");
}
