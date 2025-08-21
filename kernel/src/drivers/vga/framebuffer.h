#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "tty.h"


#define WHITE 0xffffffff
#define SILVER 0xffc0c0c0
#define GRAY 0xff808080
#define BGRAY 0xffC0C0C0
#define DGRAY 0xff404040
#define BLACK 0xff000000
#define PINK 0xffFF1493
#define GREEN 0xff008000
#define RED 0xff800000
#define PURPLE 0xff800080
#define ORANGE 0xffFF4500
#define CYAN 0xff008080
#define YELLOW 0xffFFD700
#define BROWN 0xffA52A2A
#define BLUE 0xff000080
#define DBLUE 0xff000030
#define BRED 0xffFF0000
#define BBLUE 0xff0000FF
#define BGREEN 0xff00FF00
#define TBLACK 0x00000000

extern void *memmove(void *, const void *, size_t);

struct Point
{
  unsigned int x;
  unsigned int y;
};

struct FrameBuffer
{
  void *base_address;
  size_t buffer_size;
  unsigned int width;
  unsigned int height;
  unsigned int pixels_per_scan_line;
};

struct TTYRenderer
{
    struct Point cursor_position;
    struct FrameBuffer *framebuffer;
    struct PSF1_FONT *psf1_font;
    
    uint32_t color;
    bool overwrite;
};

void init_tty_renderer(struct TTYRenderer *ttyrenderer, struct FrameBuffer *framebuffer, struct PSF1_FONT *psf1_font);

void tty_print(struct TTYRenderer *ttyrenderer, const char* str);
void put_char(struct TTYRenderer *ttyrenderer, char symbol, unsigned int x_offset, unsigned int y_offset);
void tty_clear(struct TTYRenderer *ttyrenderer, uint32_t color, bool reset_cursor);
void move_tty_cursor_down(struct TTYRenderer *);
void clear_tty_line(struct TTYRenderer *renderer, uint32_t color, uint32_t line);
extern struct TTYRenderer *global_tty_renderer;

#endif
