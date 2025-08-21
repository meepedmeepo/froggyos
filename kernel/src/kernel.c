#include "kernel.h"
#include "drivers/uart.h"
#include "drivers/vga/framebuffer.h"


static struct TTYRenderer r;

struct TTYRenderer *global_renderer;
//move this shit elsewhere.
char* itoa(int value, char* result, int base) {
// check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );
// Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

void kernel_init(struct FrameBuffer fb, struct PSF1_FONT *psf1_font){
  init_tty_renderer(&r,&fb, psf1_font);
  write_serial_string("tty init\n");
  global_renderer = &r;

  tty_clear(global_renderer, BLUE, true);
  write_serial_string("tty cleared\n");
  tty_print(global_renderer, "This text is very gay! \n Cum\n");
  write_serial_string("tty printed\n");

  tty_print(global_renderer, "Froggy Os: kernel init complete.\n");

  char arr[60] = "";
  char *buffer = arr;
  
  for (int i = 0; i < 31; i++) {
    char *buf = arr;
    buf = itoa(i, buf, 10);
    write_serial_string(buf);
    tty_print(global_renderer, buf);
    tty_print(global_renderer, "\n");
  }
  
    //tty_print(global_renderer, "\n");
    
    //tty_print(global_renderer, "\n");

  
  write_serial_string("\n");
  write_serial_string(itoa(global_renderer->cursor_position.y, buffer, 10));

  
    tty_print(global_renderer, "\n");
    tty_print(global_renderer, "cum twat\n");
    
    tty_print(global_renderer, "cum twat\n");
}
