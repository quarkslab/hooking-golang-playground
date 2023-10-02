#include "inserter.h"

/*
Hook insertion module
*/

// MODULE's GLOBAL VARIABLES

JOffsets OFFSETS = { \
    .OFF1JMP = 1,\
    .OFF2JMP = 9,\
    .OFF1JMPB = 1,\
    .OFF2JMPB = 9,\
    .OFFSETSTART = 10 \
};

AssemblyStubs aStubs = {
    .JUMP = { \
        // push <last-4-bytes-address>
        0x68, 0x90, 0x90, 0x90, 0x90, \
        // mov [rsp+4], <first-4-bytes-address>
        0xc7, 0x44, 0x24, 0x04, 0x90, 0x90, 0x90, 0x90, \
        // ret
        0xc3
    },
    // Stub for returning to the original function (trampoline)
    .JUMP_BACK_NEXT = {\
        0x68, 0x90, 0x90, 0x90, 0x90, \
        0xc7, 0x44, 0x24, \
        0x04, \
        0x90, 0x90, 0x90, 0x90, \
        0xc3
    },
    // Call instruction stub
    // 	  0:   41 51                   push   %r9
    //    2:   49 b9 ff ff ff ff ff    movabs $0xffffffffffff,%r9
    //    9:   ff 00 00 
    //    c:   41 ff d1                call   *%r9
    //    f:   0x41 0x59               pop %r9
    .CALL = {\
        0x41, 0x51,\
        0x49, 0xb9, 0x90, 0x90,0x90,0x90,0x90,0x90,0x90,0x90, \
        0x41, 0xff, 0xd1, \
        0x41, 0x59
    },
};


Storage storage;
char * TRAMPOLINE;
// the address of the target function 
ull* TARGET_AS_PTR;
ull target = 0x4939c0llu;


void 
initJump(){
    
    ull replace = (ull)(&changeStack);
    
    // skip the target function's stack init prologue 
    replace +=  OFFSETS.OFFSETSTART;
    int ssize = sizeof(storage.buffer);
    for (int i = 0; i < ssize; i++)
    {
        storage.buffer[i] = (char)(replace >> (ssize * i));
    }

    // copy the contents of the address buffer into the JMP 
    // instruction buffers
    // at the respective offsets (after instruction opcodes)
    memcpy(&aStubs.JUMP[OFFSETS.OFF1JMP], storage.buffer, 4);
    memcpy(&aStubs.JUMP[OFFSETS.OFF2JMP], &storage.buffer[4], 4);
}


int 
changePerms(void *addr)
{

    int pageSize;
    if ((pageSize = sysconf(_SC_PAGESIZE)) == -1)
    {
        perror("sysconf when changing permissions");
        return -1;
    }
    uintptr_t *end = (uintptr_t *)addr + 1;
    // := addr - (addr%pageSize)
    uintptr_t pageStart = (uintptr_t)addr & -pageSize; 
    if (mprotect((void *)pageStart, end - (uintptr_t *)pageStart,
                 PROT_READ | PROT_WRITE | PROT_EXEC) == -1)
    {
        return -1;
    }

    return 0;
}


void*
initStubsModule(){
    // the destination function (the hook)
    // allocate a stack for pivoting of size 32 kb (size of a thead’s stack on Linux)
    stacks.newstack = malloc(4*8192 * sizeof(char)); // 32 kB of stack
    return stacks.newstack;
}


void*
anon_alloc(size_t size) {
    int fd;
    if ((fd = open("/dev/zero", O_RDWR)) == -1) {
        return 0;
    }
    int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    // mapping will no be usable by other programs
    int flags = MAP_PRIVATE;
    void *p = mmap(0, size, prot, flags, fd, 0);
    close(fd);
    return p != MAP_FAILED ? p : 0;

} 

// The library constructor -  called when the shared library is loaded
__attribute__((constructor)) 
static void 
init(void)
{	

    TARGET_AS_PTR = (ull*)(target + (ull)OFFSETS.OFFSETSTART);

    // copy the instructions which will be overwritten
    memcpy((void*)storage.backup, (void*)TARGET_AS_PTR, NBACKI);
    
    // allocate heap segment of size (call addr) + NBACKI + jump back next
    int pageSize; 
    if ((pageSize = sysconf(_SC_PAGESIZE)) == -1)
    {
        perror("sysconf when finding page size");
        return;
    }
    int sizeHeap = (sizeof(aStubs.CALL) + \
     NBACKI + sizeof(aStubs.JUMP_BACK_NEXT))*sizeof(char);
    
    // perform page-size aligning
    if (sizeHeap > pageSize) {
        sizeHeap = (sizeHeap & (-pageSize)) + pageSize;
    } else {
        int tmp = pageSize - sizeHeap;
        sizeHeap = tmp + pageSize;   
    }
    if (!(TRAMPOLINE = (char*)anon_alloc(sizeHeap))) {
        perror("Couldn't allocate page for the trampoline section");
        return;
    }
    
    // Initialize the address in the call stub  
    // which is to be inserted at the beginning of the TRAMPOLINE.
    // This is going to be the stack pivot stub
    uintptr_t addr = (uintptr_t)changeStack;
    for (int i = 0; i < 8; i++)
    {
        storage.buffer[i] = (char)(addr >> (sizeof(uintptr_t) * i));
    }
    // copy the contents into the CALL stub
    memcpy(&aStubs.CALL[4], storage.buffer, sizeof(uintptr_t));

    // copy the CALL stub onto the TRAMPOLINE section
    memcpy((void*)TRAMPOLINE, (void*)aStubs.CALL, sizeof(aStubs.CALL));
    
    // copy the backup code at the TRAMPOLINE after the CALL stub
    memcpy((void*)(TRAMPOLINE+sizeof(aStubs.CALL)),
    (void*)(storage.backup), NBACKI);
    
    // initialize the JUMP_BACK stub with the address 
    // of the next instruction in the target function 
    // after the desired initial offset + the number of backup instruction
    addr = (uintptr_t)((char*)TARGET_AS_PTR + NBACKI);
    for (unsigned long i = 0; i < sizeof(storage.buffer); i++)
    {
        storage.buffer[i] = (char)(addr >> (sizeof(storage.buffer) * i));
    }

    memcpy(&aStubs.JUMP_BACK_NEXT[OFFSETS.OFF1JMPB], storage.buffer, 4);
    memcpy(&aStubs.JUMP_BACK_NEXT[OFFSETS.OFF2JMPB], &storage.buffer[4], 4);
    // copy the JUMP_BACK stub onto the trampoline
    memcpy((void*)(TRAMPOLINE+sizeof(aStubs.CALL)+NBACKI),
    (void*)(aStubs.JUMP_BACK_NEXT), sizeof(aStubs.JUMP_BACK_NEXT));

    
    // initialize the jump stub which is to be inserted at the beginning of the function
    addr = (uintptr_t)(TRAMPOLINE) ;
    for (int i = 0; i < 8; i++)
    {
        storage.buffer[i] = (char)(addr >> (8 * i));
    }
    memcpy(&aStubs.JUMP[OFFSETS.OFF1JMP], storage.buffer, 4);
    memcpy(&aStubs.JUMP[OFFSETS.OFF2JMP], &storage.buffer[4], 4);


    // change permissions - text segment +w
    if (changePerms((void *)((ull)TARGET_AS_PTR-OFFSETS.OFFSETSTART)) == -1)
    {
        perror("Error changing text section permissions");
        return;
    }

    // init the stubs module
    if (!initStubsModule()){
        perror("malloc new stack");
        return;
    }

    // copy the jump stub at the target function's location
    memcpy((void *)TARGET_AS_PTR, aStubs.JUMP, sizeof(aStubs.JUMP));
    printf("Injection completed, try your luck ;)\n");
}
