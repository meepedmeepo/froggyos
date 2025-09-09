#ifdef _K_IDT_H
#define _K_IDT_H

typedef struct {
  uint16_t isr_low;//lower 16 bits of ISR's address
  uint16_t kernel_cs;//GDT segement selector CPU will load into CS before calling ISR
  uint8_t ist;//IST in TSS that CPU will load into RSP.
  uint8_t attributes;//Type and attributes
  uint16_t isr_mid;//higher 16 btis of the lower 32 bits of ISR address
  uint32_t isr_high;//upper 32 bits of ISR address.
  uint32_t reserved;//should be zero.
} __attribute__((packed)) idt_entry_t;

__attribute__((aligned(0x10)))
static idt_entry_t idt[256];

typedef struct {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed)) idtr_t;

static idtr_t idtr;

__attribute__((noreturn))
void exception_handler(void);
void exception_handler() {
  __asm__ volatile ("cli; hlt");//Halts program execution.
}



#endif
