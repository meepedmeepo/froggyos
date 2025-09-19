#ifndef _KLIBC_PRINTF_H
#define _KLIBC_PRINTF_H

//SAFETY!
// . TTY needs to be initialised before this is called.
// . Resulting formatted string needs to be less than 1024 characters.
void printf(char *fmt, ...);

//SAFETY!
// Buffer needs to be able to contain whole size of result string
// , else it will overflow.
void sprintf(char *fmt, char *buffer, ...);

//SAFETY! see printf above ^
// Prints formatted string to serial output.
void serial_printf(char *fmt, ...);

//SAFETY! see printf ^
// Works exactly the same as printf but adds a new line
// character to the end of the output.
void println(char *fmt, ...);
#endif
