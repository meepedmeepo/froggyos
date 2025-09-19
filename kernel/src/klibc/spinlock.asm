
section .text
global _acquire_spinlock
;.preamble:
;        mov qword rax, rdi

_acquire_spinlock:
        lock bts dword [rdi],0
        jc .spin_with_pause
        mov dword rax, 0
        ret

.spin_with_pause:
        pause
        test dword [rax],1
        jnz .spin_with_pause
        jmp _acquire_spinlock

global _release_spinlock
;.preamble:
;        mov qword rax, rdi
_release_spinlock:
        mov dword [rdi],0
       ; mov dword rax,0
        ret
