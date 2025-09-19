#include "kernel.h"
#include "arch/interrupts/idt.h"
#include "arch/paging/paging.h"
#include "drivers/uart.h"
#include "drivers/vga/framebuffer.h"
#include "klibc/bucketalloc.h"
#include "limine.h"
#include "memory/framealloc.h"
#include "memory/heap.h"
#include "memory/memorymap.h"
#include <stdint.h>

#include "klibc/string/printf.h"
#include "memory/vmm.h"

static struct TTYRenderer r;

struct TTYRenderer *global_renderer;
extern char* itoa(int value, char* result, int base); 
extern char* ultoa(uint64_t value, char* result, int base);
  
void kernel_init(struct FrameBuffer fb, struct PSF1_FONT *psf1_font, void *memmap){
    init_tty_renderer(&r,&fb, psf1_font);
    write_serial_string("tty init\n");
    global_renderer = &r;

    tty_clear(global_renderer, BLUE, true);

    tty_print(global_renderer, "Froggy Os: vga driver init complete..\n");

    idt_assemble();
    tty_print(global_renderer, "IDT Initialised.\n");
    write_serial_string("IDT Init.\n");

    //Testing exception handler.
    //uint64_t *ad = 0x0;
    //*ad = 69;

    //Setup initial frame allocator.
    void *framesToAllocate = read_memory_map(memmap);
    //TODO change this pls, only temporary and could blow up in my face easily if memory regions are different at all.
    uint64_t frameAllocAddress = ((struct limine_memmap_response *)memmap)->entries[1]->base + PHYSICAL_ADDRESS_OFFSET;
    
    struct FreeFrameList* frameList = init_frame_list((void *)frameAllocAddress, framesToAllocate);

    printf("PMM init complete\n");
    
    write_serial_string("Next Free Address: 0x");
    char res[60];
    write_serial_string(ultoa(frameList->nextFree->physAddress, res, 16));
    write_serial_string("!\n");

    VirtAddress virtAddr = (VirtAddress)0xffff80000008c990;
    write_serial_string("VirtAddr written\n");

    uint64_t pml4Addr = (uint64_t)read_cr3() & ~0xFFFULL;

    pml4Addr += PHYSICAL_ADDRESS_OFFSET;
    
    write_serial_string("\n");
    
    map_to_page(virtAddr, pop_frame_list(frameList), frameList, NULL);
    //create_kernel_page_tables(frameList);
    *virtAddr = 69420;

    printf("\n");
    
    uint64_t beans = 3622983012831092324;
    char *testt = "wanker";
    char *frog = "frog";
    printf("This is a test and this is a number %ulh beans test, %s, I love %s\n", beans, testt, frog);

    
    serial_printf("This is a test and this is a number %ulh beans test, %s, I love %s\n", beans, testt, frog);
   // __asm__("int $0x03");

    tty_break();
    printf("F R O G E");
    printf("beans>");

    init_heap();
    
    init_vmm(frameList);
    serial_printf("Init VMM\n");
    printf("Init VMM.");

    //grow_heap(0x1000);
    
    kmalloc_init(0x1000);
    serial_printf("Kmalloc init\n");

    uint64_t *number = (uint64_t *)kmalloc(sizeof(uint64_t));

    serial_printf(" \nmalloced address %p \n", (uintptr_t)number);
    
    *number = 10023;

    printf("Number = %ul", *number);

    kfree(number);
}
