#include <sys/stat.h>

int _close(int file) { return -1; }
 
int _fstat(int file, struct stat *st) {
 return -1;
}
 
int _isatty(int file) { return 1; }
 
int _lseek(int file, int ptr, int dir) { return 0; }
 
int _open(const char *name, int flags, int mode) { return -1; }
 
int _read(int file, char *ptr, int len) {
	return -1;
}
 

unsigned int  *heap_end = 0;
caddr_t _sbrk(int incr) {
 print("sbrk", 0,10,255,0,0);
 extern unsigned int heap_low; 
 extern unsigned int heap_top; 
 unsigned int  *prev_heap_end;
 
 if (heap_end == 0) {
  heap_end = &heap_low;
 }
 prev_heap_end = heap_end;
 
 if (heap_end + incr > &heap_top) {
  
  return (caddr_t)0;
 }
 
 heap_end += incr;
 return (caddr_t) prev_heap_end;
 }
 
int _write(int file, char *ptr, int len) {

 return len;
 }