
#include "hook.h"

/* Hook module */

/*
Convert ascii string (a-z) to uppercase  (A-Z)
*/
void 
toUpper(GoString str) {
    char *data = str.p;
    for (int i=0; i < str.n; i++){
            data[i] -=  (data[i] >= 97 && data[i] <= 122 )? 32 : 0;
    }
} 


