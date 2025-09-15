#include "kernel.h"
#include "arch/interrupts/idt.h"
#include "arch/paging/paging.h"
#include "drivers/uart.h"
#include "drivers/vga/framebuffer.h"
#include "limine.h"
#include "memory/framealloc.h"
#include "memory/memorymap.h"
#include <stdint.h>


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

char* ultoa(uint64_t value, char* result, int base) {
  
    if (base < 2 || base > 36) { *result = '\0'; return result; }
    char* ptr = result, *ptr1 = result, tmp_char;
    uint64_t tmp_value;
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

void kernel_init(struct FrameBuffer fb, struct PSF1_FONT *psf1_font, void *memmap){
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


    idt_assemble();
    tty_print(global_renderer, "IDT Initialised.\n");
    write_serial_string("Idt Init.\n");
    uint64_t *ad = 0x0;
    *ad = 69;

    //Setup initial frame allocator.
    void *framesToAllocate = read_memory_map(memmap);
    //TODO change this pls, only temporary and could blow up in my face easily if memory regions are different at all.
    uint64_t frameAllocAddress = ((struct limine_memmap_response *)memmap)->entries[1]->base + PHYSICAL_ADDRESS_OFFSET;
    
    struct FreeFrameList* frameList = init_frame_list((void *)frameAllocAddress, framesToAllocate);

    write_serial_string("Next Free Address: 0x");
    char res[40];
    write_serial_string(ultoa(frameList->nextFree->physAddress, res, 16));
    write_serial_string("!\n");

    VirtAddress virtAddr = (VirtAddress)0xffff80000008c990;
    write_serial_string("VirtAddr written\n");

    uint64_t pml4Addr = (uint64_t)read_cr3() & ~0xFFFULL;

    pml4Addr += PHYSICAL_ADDRESS_OFFSET;
    
    char x[40];
    write_serial_string(ultoa(pml4Addr, x, 16));
    write_serial_string("\n");
    uint64_t val = *(uint64_t *)pml4Addr;

    char y[40];
    write_serial_string(ultoa(val, y, 10));

    write_serial_string("\n");
    
    map_to_page(virtAddr, pop_frame_list(frameList), frameList, NULL);
    //create_kernel_page_tables(frameList);
    *virtAddr = 69420;


    char p[20];
    tty_print(global_renderer, ultoa(*virtAddr, p, 10));


   // __asm__("int $0x03");
}
