#ifndef __COMMON__ERROR
#define __COMMON__ERROR

#include <assert.h>
#include <stdio.h>


#define RET_OK              (0)
#define RET_PARSE_CORRUPT   (2)
#define RET_PARSE_FAIL      (4)
#define RET_READ_ERR        (8)
#define RET_UNSUPPORTED     (16)


#define CHECK_ERRORS(r)     if (r != RET_OK) {\
                                fprintf(stderr, "%s:%u Error-%u\n", __FILE__, __LINE__, r);\
                                return r;\
                            }

#ifdef NDEBUG
#define DEBUG(M, ...)
#else
#define DEBUG(M, ...) fprintf(stderr, "DEBUG %s:%-4d  " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif


#endif//__COMMON__ERROR
