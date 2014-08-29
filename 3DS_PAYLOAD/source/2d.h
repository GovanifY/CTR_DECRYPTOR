#ifndef D2_H
#define D2_H

#define FCRAM_BASE 0x20000000
#define TOP_ASPECT_X 0x5
#define TOP_ASPECT_Y 0x3
#define TOP_HEIGHT 240
#define TOP_WIDTH 400
#define TOP_LEFT_FRAME1 0x20184E60
#define TOP_LEFT_FRAME2 0x201CB370

#include "3dstypes.h"
#include "math.h"

void paint_pixel(int x, int y, char r, char g, char b, int screen);
void paint_letter(char letter,int x,int y, char r, char g, char b, int screen);
void paint_word(char* word, int x,int y, char r, char g, char b, int screen);
void paint_word_vert(char* word, int x,int y, char r, char g, char b, int screen);
void paint_buffer(u8* file, point f_dim, point offset, int screen);
void paint_byte_pixel(unsigned char byte, int x, int y, int screen);
void paint_sprite(unsigned char* sheet, point f_dim, point offset, int screen, int height, int width, int xstart, int ystart);
void paint_square(int x, int y, char r, char g, char b, int h, int w, int screen);


//DLH
void blank(int x, int y, int xs, int ys);
void square(int x, int y, int xs, int ys);

#endif