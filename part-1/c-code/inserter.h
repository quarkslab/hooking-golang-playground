#ifndef INSERTER_H
#define INSERTER_H

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "stubs.h"


typedef unsigned long long ull;


// number of bytes which will be backed up when inserting the JUMP stub (program dependent)
#define NBACKI 14

// EXTERNAL VARIABLES 
// address of the stack backup structure from stub module
extern Stacks stacks;

// Module custom type structures

// offsets for the addresses in the jump stubs
typedef struct offsets {
	// offsets for the JUMP stub
	unsigned char OFF1JMP;
	unsigned char OFF2JMP;
	// offsets for the JUMP back stub
	unsigned char OFF1JMPB;
	unsigned char OFF2JMPB;
	// offset for the insertion of the JUMP stub at the target's location (target+OFFSETSTART)
	// 14
	unsigned char OFFSETSTART;
} JOffsets;


typedef struct stubs {
	unsigned char JUMP[14];
	unsigned char JUMP_BACK_NEXT[16];
	unsigned char CALL[17];
} AssemblyStubs;


typedef struct storage {
	// buffer used to store 64-bit addresses
	unsigned char buffer[8]; 
	// buffer which will hold the instructions
	// which will be overwritten by the jump stub
	unsigned char backup[NBACKI];
} Storage;


/*
Fills jump stub array with the address to jump to.
*/
void initJump();

/*
Change the page permission of a memory region.

@param addr An address inside the page which permissions
will be changed.
*/
int changePerms(void *addr);

/*
Initialize the stack which will be used for stack pivot 
in the stubs module.
*/
void* initStubsModule();

/* 
Allocate an anonymous page of memory which will be used for 
the trampoline logic. It's done using the POSIX standard 
(allocating a memory page and filling it with 0). 
The allocation must be on a page boundary (i.e size % 4096 == 0).

@param size - the size of the allocation
*/
void* anon_alloc(size_t size);
#endif