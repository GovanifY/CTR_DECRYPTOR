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
ldr r0, =0xFFFF8001
ldr r1, =0x080C3EE0
mov r2, #0x2000
svc 0x54

  BL c_entry
  B .

Interrupt_Handler:
  BL IRQHandler
  B .
