#include "isr.h"
#include "../../drivers/uart.h"
#include "../../drivers/vga/framebuffer.h"

extern struct TTYRenderer *global_renderer;

void log_isr_exception(isr_stack_frame_t *frame);


void isr_exception_handler(isr_stack_frame_t *frame) {
  tty_print(global_renderer, "Emergency Frog Situation!!!\n");
  write_serial_string("Exception Occured! \n");

  for(;;) {
  asm("hlt");
  }
}


void log_isr_exception(isr_stack_frame_t *frame) {
  frame->base_frame.vector; 
}
