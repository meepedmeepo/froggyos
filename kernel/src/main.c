#ifndef _KMAIN_C
#define _KMAIN_C

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"
#include "kernel.h"
#include "drivers/vga/tty.h"
#include "drivers/vga/framebuffer.h"
#include "drivers/uart.h"
// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0,   
};

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;


// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

bool checkStringEndsWith(const char *str, const char *end) {
    const char *_str = str;
    const char *_end = end;

    while (*str != 0 ) {
        str++;
    }
    str--;

    while (*end != 0) {
        end++;
    }
    end--;

    while (true) {
        if (*str != *end) {
            return false;
        }

        str--;
        end--;

        if (end==_end || (str==_str && end==_end)) {
            return true;
        }

        if (str == _str) {
            return false;
        }
    }
    return true;
}

char *strstr(const char *haystack, const char *needle)
{
  const char *h = haystack, *n = needle;
  for (;;) {
    if ( !*n ) return (char *) haystack;
    if ( !*h ) return NULL;
    if ( *h++ == *n++) continue;
    h = ++haystack;
    n = needle;
  }
}

struct limine_file* getFile(const char* name) {
    
    struct limine_module_response *module_response = module_request.response;
    if (module_request.response == NULL) {
        write_serial_string("get file failed!");
        hcf();
    }

    for (size_t i = 0; i < module_response->module_count; i++) {
        struct limine_file *f = module_response->modules[i];
        write_serial_string(f->path);
        if (strstr(f->path, name) != NULL) {
            write_serial_string("Found file!");
            return f;
        }
    }

    return NULL;
}


void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = (uint8_t *restrict)dest;
    const uint8_t *restrict psrc = (const uint8_t *restrict)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
        || framebuffer_request.response->framebuffer_count < 1) {
        
            hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    struct FrameBuffer fb;
    {
        fb.base_address = framebuffer->address;
        fb.width = framebuffer->width;
        fb.height = framebuffer->height;
        fb.pixels_per_scan_line = framebuffer->pitch / 4;
        fb.buffer_size = framebuffer->height * framebuffer->pitch;
    }

    init_serial();

    write_serial_string("Hello Coomers!");

    struct PSF1_FONT font;
    {
        const char *fName = "zap-vga16.psf";
        struct limine_file *file = getFile(fName);
        if (file == NULL) {
            hcf();
        }
        
        font.psf1_header = (struct PSF1_HEADER *)file->address;
        if (font.psf1_header->magic[0] != 0x36 || font.psf1_header->magic[1] != 0x04) {
            hcf();
        }

        font.glyph_buffer = (void *)((uint64_t)file->address + sizeof(struct PSF1_HEADER));
    }

    write_serial_string("font assigned\n");
    
  // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    for (size_t i = 0; i < 100; i++) {
        volatile uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }
        
    kernel_init(fb,&font);
    write_serial_string("kernel init complete\n");
    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    /* for (size_t i = 0; i < 100; i++) {
        volatile uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    } */

    // We're done, just hang...
    hcf();
}
                
#endif
