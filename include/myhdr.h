/*
   Standard header file used by nearly all of our example programs.
*/
#define MAX_ARGS 64  // 或其他適當的值
#ifndef MY_HDR_H
#define MY_HDR_H /* Prevent accidental double inclusion */

#include <stdio.h>  /* Standard I/O functions */
#include <stdlib.h> /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <assert.h>
#include <errno.h>     /* Declares errno and defines error constants */
#include <stdbool.h>   /* 'bool' type plus 'true' and 'false' constants */
#include <string.h>    /* Commonly used string-handling functions */
#include <sys/types.h> /* Type definitions used by many programs */
#include <unistd.h>    /* Prototypes for many system calls */

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

typedef enum { FALSE,
               TRUE } Boolean;

#endif
// extern char userFile[20];  // 宣告 userFile 變數
