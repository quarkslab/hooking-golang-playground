bits 64
; backup
push r9
; the address which will be called (mutex stub)
mov r9, 0x0000ffffffffffff
call r9
; this instruction will be inserted at the end of trampoline
; to restore the state 
pop r9