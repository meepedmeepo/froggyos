#ifndef _KERNEL_INIT_H
#define _KERNEL_INIT_H

#include "drivers/vga/tty.h"
#include "drivers/vga/framebuffer.h"
void kernel_init(struct FrameBuffer fb, struct PSF1_FONT* psf1_font);

#endif
