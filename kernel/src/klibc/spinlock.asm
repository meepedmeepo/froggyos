
section .text
global _acquire_spinlock
pream:
        push rax
        mov qword rax, rdi

_acquire_spinlock:
        lock bts dword [rdi],0
        jc .spin_with_pause
        pop rax
        ret

.spin_with_pause:
        pause
        test dword [rdi],1
        jnz .spin_with_pause
        jmp _acquire_spinlock

global _release_spinlock
preamble:
        push rax
        mov qword rax, rdi
release_spinlock:
        mov dword [rdi],0
        pop rax
        ret
