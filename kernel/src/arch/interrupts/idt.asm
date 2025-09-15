global idt_reload
idt_reload:
        push rbp
        mov rbp, rsp
        pushfq
        cli        ;disable interrupt flag
        lidt [rdi] ;reload idt from first function argument
        popfq
        pop rbp
        sti
        retq
