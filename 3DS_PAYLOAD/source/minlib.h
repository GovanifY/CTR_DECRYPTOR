#ifndef MINILIB_H
#define MINILIB_H

#define TICKS_PER_SEC 0x80C0000
#define TICKS_PER_MSEC (TICKS_PER_SEC/1000)
#define TICKS_PER_USEC (TICKS_PER_MSEC/1000)

#include "3dstypes.h"

typedef unsigned int size_t;

void * memcpy(void *out, const void *in, int size);
int strlen(const char *text);
//bool strcmp(const char *text, const char *text1);
void * memset(void *ptr, int c, size_t size);
uint64_t GetSystemTick(void);
void nsleep(uint64_t ns);
void usleep(uint64_t us);
void msleep(uint64_t ms);
void sleep(uint64_t s);

#endif