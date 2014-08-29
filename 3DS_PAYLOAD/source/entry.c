#include "main.h"

void c_entry() {
  main();
}

void IRQHandler (void)
{
	
    /*volatile unsigned int *base = (unsigned int *) 0x80000000;
    if (*base == 1)          // which interrupt was it?
    {
        C_int_handler();     // process the interrupt
    }
    *(base+1) = 0;           // clear the interrupt
    */
}
