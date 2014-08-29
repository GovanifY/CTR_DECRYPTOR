.arm
.section INTERRUPT_VECTOR, "x"
.global _Reset
_Reset:
  B Reset_Handler /* Reset */
  B . /* Undefined */
  B . /* SWI */
  B . /* Prefetch Abort */
  B . /* Data Abort */
  B . /* reserved */
  B Interrupt_Handler /* IRQ */
  B . /* FIQ */

Reset_Handler:
  //LDR sp, =stack_top
  BL c_entry
  B .

Interrupt_Handler:
  BL IRQHandler
  B .
