#ifndef STR_H
#define STR_H
#define _GNU_SOURCE

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

typedef long long ll;

typedef struct GoString {  
    char *p; 
    ptrdiff_t n; 
} GoString;

// Functions 
void toUpper(GoString str);

#endif