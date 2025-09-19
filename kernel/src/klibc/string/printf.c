#include "printf.h"
#include "itoa.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#include "../../drivers/vga/framebuffer.h"
#include "../../drivers/uart.h"

extern struct TTYRenderer* global_renderer;

#define PRINTBUFLEN 1024

char _print_buffer[PRINTBUFLEN];

extern void *memset(void *, int, unsigned long);

//Internal print to buffer func.
// SAFETY!
// Calling function MUST call va_end after invocation of this.
void _vprintf(char *fmt, char *buffer, va_list args) {
    while (*fmt) {
      if (*fmt == '%') {

        fmt++;
        
        //Is format specifier
        switch (*fmt) {
        case ' ': //same as below so allow to fall into it.
        case '%':
          //actually just want to use a % not a format
          *buffer = '%';
          fmt++;
          buffer++;
          break;

        case 's'://argument is a string to insert.
          fmt++;

          char *str = va_arg(args, char*);

          while(*str) {
            *buffer = *str;
            str++;
            buffer++;
          }
          
          break;

        case 'i':
        case 'p':
        case 'u':
        case 'h':
        case 'l':
        {
          bool finished = false;
          int base = 10;
          bool sign = true;
          int length = 32;
          bool isPtr = false;
          do {
            switch(*fmt){
              case 'i'://32 bit int
                length = 32;
                fmt++;
                break;
              case 'p'://64 bit pointer
                base = 16;
                isPtr = true;
                sign = false;
                length = 64;
                finished = true;
                fmt++;
                break;
                
              case 'u'://unsigned
                sign = false;
                fmt++;
                break;
              case 'h'://hexadecimal
                base = 16;
                fmt++;
                break;
              case 'l'://64 bit.
                length = 64;
                fmt++;
                break;
              default:
                finished = true;
                break;
            }
          } while (!finished);

          char res[100] = {0};

          memset(res,0, sizeof(char) * 100);

          if (isPtr) {
            uintptr_t val = va_arg(args, uintptr_t);
            utoa((uint64_t)val, res, 16);
          }else if (sign == false && length == 32) {
            uint32_t val = va_arg(args, uint32_t);
            utoa((uint32_t)val, res, base);
          } else if (sign == true && length == 32) {
            int32_t val = va_arg(args, int32_t);
            itoa((int64_t)val, res, base);
          } else if (sign == false && length == 64) {
            uint64_t val = va_arg(args, uint64_t);
            utoa(val, res, base);
          } else if (sign == true && length == 64) {
            int64_t val = va_arg(args, int64_t);
            itoa(val, res, base);
          }

          //Insert result into buffer.
          char *res_ptr = res;
          if (base == 16) {
            *buffer = '0';
            buffer++;
            *buffer = 'x';
            buffer++;
          }
          while (*res_ptr != '\0') {
            *buffer = *res_ptr;
            buffer ++;
            res_ptr++;
          }
          
          break;//end of case  
        }
        
        default:
            break;
        }
      } else {
        //No formatting required.
        *buffer = *fmt;
        buffer++;
        fmt ++;
      }
    }
}


void printf(char *fmt, ...) {
  va_list args;

  char buffer[1024] = {0};
  memset(buffer,0, sizeof(char) * 1024);

  va_start(args, fmt);

  _vprintf(fmt, buffer, args);

  tty_print(global_renderer, buffer);

  va_end(args);
}

void sprintf(char *fmt, char *buffer, ...) {
  va_list args;

  va_start(args, buffer);
  
  _vprintf(fmt, buffer, args);
  
  va_end(args);
}

void serial_printf(char *fmt, ...) {
  va_list args;

  char buffer[1024] = {0};
  
  memset(buffer,0, sizeof(char) * 1024);
  va_start(args, fmt);

  _vprintf(fmt, buffer,args);

  va_end(args);

  write_serial_string(buffer);
}


void println(char *fmt, ...) {
  va_list args;

  char buffer[1024] = {0};
  memset(buffer,0, sizeof(char) * 1024);

  va_start(args, fmt);

  _vprintf(fmt, buffer, args);

  tty_print(global_renderer, buffer);
  tty_break();
  va_end(args);
}
