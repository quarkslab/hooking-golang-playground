#include "stubs.h"

/* Assembly stubs module */

/* Variables and module globals */

// Stack pivot global variables.
Stacks stacks = {.stackbackup = 0, .newstack = 0};
void* stackbackupp = &stacks.stackbackup;
void* newstackp = &stacks.newstack;

// The address of the hook (INIT) 
void* funcToCallp = (void*)toUpper;


// Reminder:
//asm("assembly code" : output operands : input operands : clobbered registers);


void 
changeStack()  {
    __asm__ volatile("CHANGE_STACK:");
    // save current stack
    __asm__ volatile("mov %0, %%r9"
                    : 
                    : "m"(stackbackupp)
                    :"rax", "rdx", "rdx", "rbx", "rcx", "rsi", "rdi", "r8", "r10", "r11", "r10", "r12", "r14", "r13", "r15"
                    ); 
    __asm__ volatile("mov %rsp, (%r9)");
    // get the new stack pointer and pivot the stack
    __asm__ volatile("mov %0, %%r9"
                    : 
                    : "m"(newstackp) 
                    :"rax", "rdx", "rdx", "rbx", "rcx", "rsi", "rdi", "r8", "r10", "r11", "r10", "r12", "r14", "r13", "r15"
                    );    
    __asm__ volatile(
        "xchg (%r9), %rsp;"
        "jmp SAVE_CTX" // we proceed to the SAVE_CTX 
    ); 
}


void 
saveCtx() {
    __asm__ volatile("SAVE_CTX:");
    // get the r9 register which was previously stored on the stack (enter region)
    __asm__ volatile("mov %0, %%r9"
                    : 
                    : "m"(stackbackupp)
                    : "rax", "rdx", "rdx", "rbx", "rcx", "rsi", "rdi", "r8", "r10", "r11", "r10", "r12", "r14", "r13", "r15"
                    ); 
    
    __asm__ volatile("mov (%r9), %r9"); 
    // Save the current context (almost all CPU registers) on the new stack 
    // The registers RAX, RCX, RDX, R8, R9, R10, R11 are considered volatile (caller-saved)
    // The registers RBX, RBP, RDI, RSI, RSP, R12, R13, R14, and R15 are considered nonvolatile (callee-saved).
    __asm__ volatile(
    "push %rax;"
    "push %rcx;"
    "push %rdx;"
    "push %r8;"
    "push %r9;"
    "push %r10;"
    "push %r11;"
    "push %rdi;"
    "push %rsi;"
    "push %rsi;" // for aligning purposes (the stack should be 8-byte aligned)
    );
    // we proceed to ABI switch (Go->C)
    __asm__ volatile ("jmp ABI_SWITCH");
}

void 
abiSwitch() {
    __asm__ volatile("ABI_SWITCH:");

    __asm__ volatile(
       
        "mov %rax, %rdi;"
        "mov %rbx, %rsi;"
        // perform the ABI switch (context dependent)
        "jmp CALL_C_FUNC"
    );
}

void 
callCFunc()  {
    __asm__ volatile("CALL_C_FUNC:");
    
    __asm__ volatile("mov %0, %%r9"
                     : 
                     :"m"(funcToCallp)
                     : "rax", "rdx", "rdx", "rbx", "rcx", "rsi", "rdi", "r8", "r10", "r11", "r10", "r12", "r14", "r13", "r15"
                     );
    __asm__ volatile(
        "call *%r9;"
        "jmp ABI_RESTORE"
    );
}


void
abiRestore(){
    __asm__ volatile("ABI_RESTORE:");

    __asm__ volatile(
        "jmp RESTORE_CTX;"
    ); 
}

void 
restoreCtx()  {
    __asm__ volatile("RESTORE_CTX:");
    // restore the CPU registers in reverse order (LIFO)
    // this is context dependent. It's possible to have to 
    // rearrange the results in the GO ABI
    __asm__ volatile(
        "pop %rsi;" // aligning purposes
        "pop %rsi;"
        "pop %rdi;"
        "pop %r11;"
        "pop %r10;"
        "pop %r9;"
        "pop %r8;"
        "pop %rdx;"
        "pop %rcx;"
        "pop %rax;"
        "jmp RESTORE_STACK;"
    );
    
}

void 
restoreStack()  {
    __asm__ volatile("RESTORE_STACK:");

    __asm__ volatile("mov %0, %%r9"
                    : 
                    : "m"(stackbackupp)
                    : "rax", "rdx", "rdx", "rbx", "rcx", "rsi", "rdi", "r8", "r10", "r11", "r10", "r12", "r14", "r13", "r15"
                    ); 
    // pivot back the curren stack and return  
    __asm__ volatile(
        "xchg (%r9), %rsp;"
        "ret"
    );
}

