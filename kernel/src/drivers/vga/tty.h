#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct PSF1_HEADER
{
  unsigned char magic[2];
  unsigned char mode;
  unsigned char charsize;  
};

struct PSF1_FONT
{
  struct PSF1_HEADER *psf1_header;
  void *glyph_buffer;  
};

#endif
