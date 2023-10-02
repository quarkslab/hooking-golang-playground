#ifndef STUBS_H
#define STUBS_H

#include "hook.h"


/* Function declarations */

/*
Stub to change the original goroutine stack. The new stack is in newstackp and the old 
will be stored in stackbackupp.
*/
void changeStack() __attribute__((naked));
/*
Stub to save the context of the CPU registers on the new stack. Registers are saved
following the System V ABI convention for caller and callee-saved registers.
*/
void saveCtx() __attribute__((naked));

/*
The custom ABI adapter. It will transform the ABI of the 
hooked routine from Go to C
*/
void abiSwitch() __attribute__((naked));

/*
Stub to call into the hook
*/
void callCFunc() __attribute__((naked));

/*
Stub to restore the ABI from C back to Go. As this is a trampoline too hence, there is 
no result, this section jumps back to RESTORE_CTX.
*/
void abiRestore() __attribute__((naked));

/*
Stub to restore the context of the CPU registers. Registers are restored using an
LIFO  strategy.
*/
void restoreCtx() __attribute__((naked));
/*
Stub to restore the original goroutine stack. The address of the old stack is in stackbackupp.
*/
void restoreStack() __attribute__((naked));


typedef unsigned long long ull;
// Structures
typedef struct stacks {
    // address where the old stack's address will be held
    void* stackbackup;
    // address where the new stack's address will be held
    void* newstack;
    
} Stacks;

#endif