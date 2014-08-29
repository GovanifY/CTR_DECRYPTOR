#include "2d.h"
#include "font.h"
#include "memory.h"
#include "string.h"


void print(char* s,int x, int y, char r, char g, char b){
  blank(x,y,strlen(s)*8, 10);
  paint_word(s, x, y, r,g,b,TOP_LEFT_FRAME1);   
  paint_word(s, x, y, r,g,b,TOP_LEFT_FRAME2);
}

void print_vert(char* s,int x, int y, char r, char g, char b){
  paint_word_vert(s, x, y, r,g,b,TOP_LEFT_FRAME1);   
  paint_word_vert(s, x, y, r,g,b,TOP_LEFT_FRAME2);
}

void print_u8(u8 sh, int x,int y, char r, char g, char b){
  char string[3];
  byte_to_string(sh, string, 3);
  print(string,x,y,r,g,b);
}

void print_u16(u16 sh, int x,int y, char r, char g, char b){
  char string[5];
  u16_to_string(sh, string, 5);
  print(string,x,y,r,g,b);
}

void print_u32(u32 sh, int x,int y, char r, char g, char b){
  char string[17];
  u32_to_string(sh, string, 17);
  print(string,x,y,r,g,b);
}

void print_u16_bits(u16 sh, int x,int y, char r, char g, char b){
  char string[17];
  u16_to_bit_string(sh, string, 17);
  print(string,x,y,r,g,b);
}

void print_byte_bits(u16 sh, int x,int y, char r, char g, char b){
  char string[17];
  byte_to_bit_string(sh, string, 9);
  print(string,x,y,r,g,b);
}


void paint_square(int x, int y, char r, char g, char b, int w, int h, int screen){
  int x1, y1;

  for (x1 = x; x1 < x+w; x1++){
    for (y1 = y; y1 < y+h; y1++){
        paint_pixel(x1,y1,r,g,b,screen);   
    }   
  }
}

void paint_pixel(int x, int y, char r, char g, char b, int screen){
  int coord = 720*x+720-(y*3);
  write_color(coord+screen,r,g,b);
}

void blank(int x, int y, int xs, int ys){
  paint_square(x,y,255,255,255,xs,ys,TOP_LEFT_FRAME1);
  paint_square(x,y,255,255,255,xs,ys,TOP_LEFT_FRAME2);
}

void square(int x, int y, int xs, int ys){
  paint_square(x,y,0,0,0,xs,ys,TOP_LEFT_FRAME1);
  paint_square(x,y,0,0,0,xs,ys,TOP_LEFT_FRAME2);
}

void square_c(int x, int y, int xs, int ys, char r, char g, char b){
  paint_square(x,y,r,g,b,xs,ys,TOP_LEFT_FRAME1);
  paint_square(x,y,r,g,b,xs,ys,TOP_LEFT_FRAME2);
}

void paint_byte_pixel(unsigned char byte, int x, int y, int screen){
        if (x >= TOP_WIDTH) return;
        if (y >= TOP_HEIGHT) return;
        unsigned char r,g,b;

        r = byte & 0b11100000;
        r = r >> 5;
        r = r*32;

        g = byte & 0b00011100;
        g = g >> 2;
        g = g*32;

        b = byte & 0b00000011;
        b = b*64;

        if (g == 224 && r == 0 && b == 0){
          return;
        }


        paint_pixel(x,y,r,g,b,screen);
}

void paint_sprite(unsigned char* sheet, point f_dim, point offset, int screen, int height, int width, int xstart, int ystart){
  int y,x;

  for (y = ystart; y < ystart+height; y++){
    for (x = xstart; x < xstart+width; x++){
        paint_byte_pixel(sheet[(y*f_dim.x)+x],x+offset.x-xstart,y+offset.y-ystart,screen);        
    }    
  }
}

void paint_buffer(unsigned char* file, point f_dim, point offset, int screen){
  
  int x;
  int y;

  for (y = 0; y < f_dim.y; y++){
    for (x = 0; x < f_dim.x; x++){
        paint_byte_pixel(file[(y*f_dim.x)+x],x+offset.x,y+offset.y,screen);        
    }    
  }

}

void paint_letter(char letter,int x,int y, char r, char g, char b, int screen){

  int i;
  int k;
  int c;
  unsigned char mask;
  unsigned char* _letter;
  unsigned char l; 
  c=(letter-32)*8;

  for (i = 0; i < 8; i++){
    mask = 0b10000000;
    l = font[i+c];
    for (k = 0; k < 8; k++){
      if ((mask >> k) & l){
        paint_pixel(k+x,i+y,r,g,b,screen);
      }     
    }
  }
}

void paint_word(char* word, int x,int y, char r, char g, char b, int screen){
    int tmp_x =x;
    int i;
    int line = 0;

    for (i = 0; i <strlen(word); i++){
     
      if (tmp_x+8 > TOP_WIDTH) {
        line++;
        tmp_x = x;
      }
      paint_letter(word[i],tmp_x,y+(line*8),r,g,b, screen);

      tmp_x = tmp_x+8;
    }

}

void paint_word_vert(char* word, int x,int y, char r, char g, char b, int screen){
    int tmp_y =y;
    int i;
    int line = 0;

    for (i = 0; i <strlen(word); i++){
     
      if (tmp_y+8 > TOP_HEIGHT) {
        line++;
        tmp_y = y;
      }
      paint_letter(word[i],x+(line*8),tmp_y,r,g,b, screen);

      tmp_y = tmp_y+8;
    }

}