#ifndef DEBUG_H
#define DEBUG_H
#include "memory.h"
#include "2d.h"

#define REGISTER_W 1
#define PAGE_W 2
#define ALL_W 3

#define NUM_REGISTER_SAVES 20
#define REGISTER_MODE_BYTE 1
#define REGISTER_MODE_WORD 2

struct REGISTER_WATCHER {
	u32 values[NUM_REGISTER_SAVES];
	u32 address;
	u32 mode;
};

typedef struct REGISTER_WATCHER reg_watcher;

struct PAGE_WATCHER {
	u32 values[6*27];
	u32 address;
};

typedef struct PAGE_WATCHER page_watcher;

void init_debug();
void print_watcher(s32 type, u32 id);
void set_watcher_address(s32 type, u32 id, u32 address);
void update_watcher(s32 type, u32 id);
void update_all_watchers(s32 type);

//Easy Debug printing functions painting to both screens
void print(char* s,int x, int y, char r, char g, char b);
void print_vert(char* s,int x, int y, char r, char g, char b);

#endif