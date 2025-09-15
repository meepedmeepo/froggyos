#include "idt.h"
#include <stdbool.h>

__attribute__((aligned(0x1000)))
idt_entry_t __idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtr;

static bool vectors[IDT_MAX_DESCRIPTORS];

uint64_t __irq_handlers[IDT_MAX_DESCRIPTORS];

extern uint64_t isr_stub_table[];


//Kernel code descriptor offset in GDT is 40 according to limine protocol.
void idt_set_descriptor(uint8_t vector, uintptr_t isr, uint8_t flags, uint8_t ist) {
  idt_entry_t *descriptor = &__idt[vector];

  descriptor->isr_low = isr & 0xFFFF;
  descriptor->kernel_cs = 40;
  descriptor->ist = ist;
  descriptor->attributes = flags;
  descriptor->isr_mid = (isr >> 16) & 0xFFFF;
  descriptor->isr_high = (isr >> 32) & 0xFFFFFFFF;
  descriptor->reserved = 0;
}

void idt_assemble() {
  idtr.base = (uintptr_t)&__idt[0];
  idtr.limit = (uint16_t)sizeof(idtr_t) * IDT_MAX_DESCRIPTORS - 1;

  for (uint8_t vector = 0; vector < IDT_CPU_EXCEPTION_COUNT; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], IDT_DESCRIPTOR_EXCEPTION, 0);
    vectors[vector] = true;
  }

  idt_reload(&idtr);
}

void idt_free_vector(uint8_t vector) {
  idt_set_descriptor(vector, 0,0,0);
  __irq_handlers[vector] = 0;
  vectors[vector] = false;
}

uint8_t idt_allocate_vector() {
  for (uint32_t i = 0; i < IDT_MAX_DESCRIPTORS; i++) {
    if(!vectors[i]) {
      vectors[i] = true;
      return (uint8_t)i;
    }
  }

  return 0;
}
