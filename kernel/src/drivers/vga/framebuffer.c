
#include "framebuffer.h"
#include <stddef.h>
#include <stdint.h>
#include "../uart.h"

void move_tty_cursor_down(struct TTYRenderer *renderer) {
  if (renderer->cursor_position.y + renderer->psf1_font->psf1_header->charsize >= renderer->framebuffer->height - (uint32_t)renderer->psf1_font->psf1_header->charsize) {
    write_serial_string("Scroll Buffer Activated!");
    //move all of current pixel buffer upwards 16
    uint32_t *frameBufStart = renderer->framebuffer->base_address;
    uint32_t *copyRegionStart = renderer->framebuffer->base_address + renderer->framebuffer->pixels_per_scan_line * ((uint32_t)renderer->psf1_font->psf1_header->charsize * 4);
    //Number of bytes in the first line of text in framebuffer.
    size_t firstRegionSize = (size_t)(renderer->framebuffer->pixels_per_scan_line * (size_t)renderer->psf1_font->psf1_header->charsize * 4);
    //Subtract size of first region from total framebuffer size to get size of region to copy.
    size_t copySize = renderer->framebuffer->buffer_size - firstRegionSize;

    //moves framebuffer contents upwards
    memmove(frameBufStart, copyRegionStart, copySize);

    
    
  } else {
    renderer->cursor_position.y += renderer->psf1_font->psf1_header->charsize;
    renderer->cursor_position.x = 0;
  }
}

void init_tty_renderer(struct TTYRenderer *ttyrenderer, struct FrameBuffer *framebuffer, struct PSF1_FONT *psf1_font)
{
  ttyrenderer->color = WHITE;

  ttyrenderer->cursor_position.x = 0;
  ttyrenderer->cursor_position.y = 0;

  ttyrenderer->framebuffer = framebuffer;
  ttyrenderer->psf1_font = psf1_font;  
}

void tty_print(struct TTYRenderer *ttyrenderer, const char *str){
  char *chr = (char *)str;
  //write_serial_string("print started \n");
  while (*chr != 0) {
    switch(*chr) {
      case '\n':
        ttyrenderer->cursor_position.x = 0;
        move_tty_cursor_down(ttyrenderer);
        break;
      case '\t':
        ttyrenderer->cursor_position.x += 8;
        break;
      default:
        put_char(ttyrenderer, *chr, ttyrenderer->cursor_position.x, ttyrenderer->cursor_position.y);
        ttyrenderer->cursor_position.x += 8;
        break;
    }

    if (ttyrenderer->cursor_position.x + 8 > ttyrenderer->framebuffer->width)
    {
      ttyrenderer->cursor_position.x = 0;
      move_tty_cursor_down(ttyrenderer);
    }
    
    chr++;
  }
}

void put_char(struct TTYRenderer *ttyrenderer, char symbol, unsigned int x_offset, unsigned int y_offset){
  //write_serial_string("putchar! \n");
  uint32_t volatile *framebufPtr = ttyrenderer->framebuffer->base_address;
  char volatile *glyphPtr = (char *)ttyrenderer->psf1_font->glyph_buffer + (symbol * ttyrenderer->psf1_font->psf1_header->charsize);

  for (uint32_t y = y_offset; y < y_offset + ttyrenderer->psf1_font->psf1_header->charsize; y++)
  {
    uint32_t mask = 1<<7;
    for (uint32_t x = x_offset; x < x_offset + 8; x++)
    {
      if ((*glyphPtr & mask) > 0)
      {
        *(uint32_t *)(framebufPtr + x + (y * ttyrenderer->framebuffer->pixels_per_scan_line)) = ttyrenderer->color;
      }
      
        mask >>= 1;
    }
    glyphPtr++;
  }
}

void draw_char(struct TTYRenderer *renderer, unsigned char chr, int cx, int cy) {
  renderer->psf1_font->psf1_header->charsize;
}

void tty_clear(struct TTYRenderer *ttyrenderer, uint32_t color, bool reset_cursor) {
  uint64_t fbBase = (uint64_t)ttyrenderer->framebuffer->base_address;
  uint64_t pxlsPerScanLine = ttyrenderer->framebuffer->pixels_per_scan_line;
  uint64_t fbHeight = ttyrenderer->framebuffer->height;
  
  for (int64_t y = 0; y < ttyrenderer->framebuffer->height; y++)
  {
    for (int64_t x  = 0; x < ttyrenderer->framebuffer->width; x++)
    {
      *((uint32_t *)(fbBase + 4 * (x + pxlsPerScanLine * y))) = color;
    }
  }


  if (reset_cursor) {
    ttyrenderer->cursor_position.x = 0;
    ttyrenderer->cursor_position.y = 0;
  }
}

void clear_tty_line(struct TTYRenderer *renderer, uint32_t color, uint32_t line) {
  uint32_t *volatile fbBase = (uint32_t *)renderer->framebuffer->base_address;
  uint32_t scanline = renderer->framebuffer->pixels_per_scan_line;
  line *= scanline *4;

  for (uint32_t y = line; y < line + (uint32_t)renderer->psf1_font->psf1_header->charsize; y++) {
    for (uint32_t x = 0; x < renderer->framebuffer->width; x++) {
      *((uint32_t *)(fbBase + 4 * (x + scanline * y))) = color;
    }
  }
}
