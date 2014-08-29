#include <string.h>
#include "FS.h"

u32 FileOpen(wchar_t* fname)
{
	asm(
		"f_handle   = 0x20         \n\t"

		"STMFD   SP!, {R3-R12,LR}\n\t"
		"SUB     SP, SP, #0x30\n\t"

		"add    r7, r0, #0      \n\t"

		"LDR     R5, =0x809797C\n\t"
		"LDR     R5, [R5]\n\t"
		"ADD     R5, #8\n\t"
		"LDR     R1, =0x2EA0\n\t"
		"ADD     R0, R5, R1\n\t"
		"ADD     R1, SP, #8\n\t"
		"LDR     R4, =0x8061451\n\t"
		"BLX     R4\n\t"
		"MOV     R3, #0\n\t"
		"STR     R3, [SP,#0x1C]\n\t"
		"STR     R3, [SP]\n\t"
		"STR     R3, [SP,#4]\n\t"
		"ADD     R0, SP, #0x10\n\t"
		"MOV     R1, R5\n\t"
		"LDR     R2, [SP,#8]\n\t"
		"LDR     R3, [SP,#0xC]\n\t"
		"LDR     R4, =0x8063F91\n\t"
		"BLX     R4\n\t"
		"LDR     R6, [SP,#0x1C]\n\t"
		"ADD     R0, SP, #0x24\n\t"
		"MOV     R1, #4\n\t"
		"STR     R1, [R0]\n\t"
		"MOV     R1, #0x1C\n\t"
		"STR     R1, [R0,#8]\n\t"
		"add     r1, r7, #0\n\t"
		"STR     R1, [R0,#4]\n\t"
		"MOV     R0, #0\n\t"
		"STR     R0, [SP, #f_handle]\n\t"
		"MOV     R3, #7\n\t"            // FILE_READ | FILE_WRITE | FILE_CREATE
		"STR     R3, [SP]\n\t"
		"STR     R0, [SP,#4]\n\t"
		"ADD     R1, SP, #0x20\n\t"
		"MOV     R2, #0\n\t"
		"ADD     R3, SP, #0x24\n\t"
		"LDR            R10, =0x8084739\n\t"
		"MOV     R0, R6\n\t"

		"BLX     R10\n\t"

		"LDR     R0, [SP, #f_handle]\n\t"

		"ADD     SP, SP, #0x30\n\t"
		"LDMFD   SP!, {R3-R12,PC}\n\t"
	);
}

void FileRead(u32 file_handle, void* in_buf, u32 size)
{
	asm(
		"f_read_call   = 0x38\n\t"

		"STMFD   SP!, {R3-R12,LR}\n\t"
		"SUB     SP, SP, #0x20\n\t"

		"ADD     R3, SP, #0x10\n\t"
		"STR     R2, [SP,#0x8]\n\t"
		"LDR     R2, =0x80944C8\n\t"
		"STR     R2, [R3]\n\t"
		"STR     R1, [R3,#4]\n\t"
		"STR     R3, [SP,#0x0]\n\t"
		"MOV     R1, #0\n\t"
		"STR     R1, [SP,#0x4]\n\t"
		"ADD     R1, SP, #0xC\n\t"
		"mov     R2, #0\n\t"
		"MOV     R3, #0\n\t"
		"LDR     R4, [R0]\n\t"
		"LDR     R4, [R4, #f_read_call]\n\t"

		"BLX     R4\n\t"

		"ADD     SP, SP, #0x20  \n\t"
		"LDMFD   SP!, {R3-R12,PC}\n\t"
	);
}
//[19:15] <@sm> in FileRead one of those two arguments at the end is the offset
//[19:15] <@sm>     mov     R2, #0
//[19:15] <@sm>     MOV     R3, #0
//[19:16] <@sm> so you could write FileReadOffset and just use one of those as an argument
//[19:16] <@sm> I think it's R2 but not 100% sure never actually used it

void FileWrite(u32 file_handle, void* write_buf, u32 size)
{
	asm(
		"f_write_call   = 0x3C\n\t"

		"STMFD   SP!, {R3-R12,LR}\n\t"
		"SUB     SP, SP, #0x20\n\t"

		"ADD     R3, SP, #0x10\n\t"
		"STR     R2, [SP,#0x8]\n\t"
		"LDR     R2, =0x8094490\n\t"
		"STR     R2, [R3]\n\t"
		"STR     R1, [R3,#4]\n\t"
		"STR     R3, [SP,#0x0]\n\t"
		"MOV     R1, #0\n\t"
		"STR     R1, [SP,#0x4]\n\t"
		"ADD     R1, SP, #0xC\n\t"
		"mov     R2, #0\n\t"
		"MOV     R3, #0\n\t"
		"LDR     R4, [R0]\n\t"
		"LDR     R4, [R4, #f_write_call]\n\t"

		"BLX     R4\n\t"

		"ADD     SP, SP, #0x20  \n\t"
		"LDMFD   SP!, {R3-R12,PC}\n\t"
	);
}

void FileWriteOffset(u32 file_handle, void* write_buf, u32 size, u32 offset)
{
	asm(
		"f_write_call   = 0x3C\n\t"
		"\n\t"
		"    STMFD   SP!, {R4-R11,LR}\n\t"
		"    SUB     SP, SP, #0x20\n\t"
		"    MOV     R7, R3\n\t"
		"    \n\t"
		"    ADD     R3, SP, #0x10\n\t"
		"    STR     R2, [SP,#0x8]\n\t"
		"    LDR     R2, =0x8094490\n\t"
		"    STR     R2, [R3]\n\t"
		"    STR     R1, [R3,#4]\n\t"
		"    STR     R3, [SP,#0x0]\n\t"
		"    MOV     R1, #0\n\t"
		"    STR     R1, [SP,#0x4]\n\t"
		"    ADD     R1, SP, #0xC\n\t"
		"    @mov     R2, #0x10\n\t"
		"    @ldr     r2, =0xFFFF0\n\t"
		"    MOV     R2, R7\n\t"
		"    MOV     R3, #0@x1000\n\t"
		"    LDR     R4, [R0]\n\t"
		"    LDR     R4, [R4, #f_write_call]\n\t"
		"    \n\t"
		"    BLX     R4\n\t"
		"    \n\t"
		"    ADD     SP, SP, #0x20   \n\t"
		"    LDMFD   SP!, {R4-R11,PC}\n\t"
	);
}
