/*---------------------------------------------------------------------------------

	Interrupt registers and vector pointers

	Copyright (C) 2005
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.


---------------------------------------------------------------------------------*/

/*! \file interrupts.h
    \brief nds interrupt support.
*/

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "3dstypes.h"


//! values allowed for REG_IE and REG_IF
enum IRQ_MASKS {
	DMAC_1_0		=	BIT(0),
	DMAC_1_1		=	BIT(1),
	DMAC_1_2		=	BIT(2),
	DMAC_1_3		=	BIT(3),
	DMAC_1_4		=	BIT(4),
	DMAC_1_5		=	BIT(5),
	DMAC_1_6		=	BIT(6),
	DMAC_1_7		=	BIT(7),
	TIMER_0			=	BIT(8),
	TIMER_1			=	BIT(9),
	TIMER_2			=	BIT(10),
	TIMER_3			=	BIT(11),
	PXI_SYNC		=	BIT(12),
	PXI_NOT_FULL	=	BIT(13),
	PXI_NOT_EMPTY	=	BIT(14),
	AES				=	BIT(15),
	SDIO_1			=	BIT(16),
	SDIO_1_ASYNC	=	BIT(17),
	SDIO_3			=	BIT(18),
	SDIO_3_ASYNC	=	BIT(19),
	DEBUG_RECV		=	BIT(20),
	DEBUG_SEND		=	BIT(21),
	RSA				=	BIT(22),
	CTR_CARD_1		=	BIT(23),
	CTR_CARD_2		=	BIT(24),
	CGC				=	BIT(25),
	CGC_DET			=	BIT(26),
	DS_CARD			=	BIT(27),
	DMAC_2			=	BIT(28),
	DMAC_2_ABORT	=	BIT(29),
	IRQ_ALL				=	(~0)		/*!< 'mask' for all interrupt */
};


typedef enum IRQ_MASKS IRQ_MASK;


//! values allowed for REG_AUXIE and REG_AUXIF
enum IRQ_MASKSAUX {
	IRQ_I2C	=	BIT(6),	/*!< I2C interrupt mask (DSi ARM7)*/
	IRQ_SDMMC = 	BIT(8)  /*!< Sdmmc interrupt mask (DSi ARM7)*/
};

/*!
	\brief returns the mask for a given timer.

	\param n the timer.

	\return the bitmask.
*/
#define IRQ_TIMER(n) (1 << ((n) + 3))

//! maximum number of interrupts.
#define MAX_INTERRUPTS  25

/*! \def REG_IE

    \brief Interrupt Enable Register.

	This is the activation mask for the internal interrupts.  Unless
	the corresponding bit is set, the IRQ will be masked out.
*/
#define REG_IE	(*(vuint32*)0x10001000)

//AUXIE ? HUH
//#define REG_AUXIE	(*(vuint32*)0x04000218)

/*! \def REG_IF

    \brief Interrupt Flag Register.

	Since there is only one hardware interrupt vector, the IF register
	contains flags to indicate when a particular of interrupt has occured.
	To acknowledge processing interrupts, set IF to the value of the
	interrupt handled.

*/
#define REG_IF	(*(vuint32*)0x10001004)
//AUXILLARY ? 
//#define REG_AUXIF	(*(vuint32*)0x0400021C)

/*! \def REG_IME

    \brief Interrupt Master Enable Register.

	When bit 0 is clear, all interrupts are masked.  When it is 1,
	interrupts will occur if not masked out in REG_IE.

*/

#define REG_IME	(*(vuint32*)0x10000FFC)


//! values allowed for REG_IME
enum IME_VALUE {
	IME_DISABLE = 0, 	/*!< Disable all interrupts. */
	IME_ENABLE = 1,		/*!< Enable all interrupts not masked out in REG_IE */
};



extern 	VoidFn	__irq_vector[];
extern	vuint32	__irq_flags[];
extern	vuint32	__irq_flagsaux[];

#define INTR_WAIT_FLAGS  *(__irq_flags)
#define INTR_WAIT_FLAGSAUX  *(__irq_flagsaux)
#define IRQ_HANDLER             *(__irq_vector)

struct IntTable{IntFn handler; u32 mask;};

/*! \fn irqInit()
	\brief Initialise the libnds interrupt system.

	This function is called internally (prior to main()) to set up irqs
    on the ARM9.  It must be called on the ARM7 prior to installing irq
    handlers.

*/
void irqInit();

/*! \fn irqSet(u32 irq, VoidFn handler)
	\brief Add a handler for the given interrupt mask.

	Specify the handler to use for the given interrupt. This only works with
	the default interrupt handler, do not mix the use of this routine with a
	user-installed IRQ handler.
	\param irq Mask associated with the interrupt.
	\param handler Address of the function to use as an interrupt service routine
	\note
	When any handler specifies using IRQ_VBLANK or IRQ_HBLANK, DISP_SR
	is automatically updated to include the corresponding DISP_VBLANK_IRQ or DISP_HBLANK_IRQ.

	\warning Only one IRQ_MASK can be specified with this function.
*/
void irqSet(u32 irq, VoidFn handler);
void irqSetAUX(u32 irq, VoidFn handler);

/*! \fn irqClear(u32 irq)
	\brief remove the handler associated with the interrupt mask irq.
	\param irq Mask associated with the interrupt.
*/
void irqClear(u32 irq);
void irqClearAUX(u32 irq);

/*! \fn irqInitHandler(VoidFn handler)
	\brief Install a user interrupt dispatcher.

	This function installs the main interrupt function, all interrupts are serviced through this routine. For most
	purposes the libnds interrupt dispacther should be used in preference to user code unless you know *exactly* what you're doing.

	\param handler Address of the function to use as an interrupt dispatcher
	\note the function *must* be ARM code
*/
void irqInitHandler(VoidFn handler);

/*! \fn irqEnable(u32 irq)
	\brief Allow the given interrupt to occur.
	\param irq The set of interrupt masks to enable.
	\note Specify multiple interrupts to enable by ORing several IRQ_MASKS.
*/
void irqEnable(u32 irq);
void irqEnableAUX(u32 irq);

/*! \fn irqDisable(u32 irq)
	\brief Prevent the given interrupt from occuring.
	\param irq The set of interrupt masks to disable.
	\note Specify multiple interrupts to disable by ORing several IRQ_MASKS.
*/
void irqDisable(u32 irq);
void irqDisableAUX(u32 irq);

/*! \fn swiIntrWait(u32 waitForSet, uint32 flags)

	\brief wait for interrupt(s) to occur

	\param waitForSet
		0: Return if the interrupt has already occured
		1: Wait until the interrupt has been set since the call
	\param flags
		interrupt mask to wait for

*/
void swiIntrWait(u32 waitForSet, uint32 flags);

/*! \fn  swiWaitForVBlank()
	\brief Wait for vblank interrupt

	Waits for a vertical blank interrupt

	\note Identical to calling swiIntrWait(1, 1)
*/
void swiWaitForVBlank(void);

/*! \fn  VoidFn setPowerButtonCB(VoidFn CB);
	\brief set callback for DSi Powerbutton press

	\param CB
		function to call when power button pressed
	\return
		the previously set callback
*/
VoidFn setPowerButtonCB(VoidFn CB);

static inline int enterCriticalSection() {
	int oldIME = REG_IME;
	REG_IME = 0;
	return oldIME;
}

static inline void leaveCriticalSection(int oldIME) {
	REG_IME = oldIME;
}


#endif //NDS_INTERRUPTS_INCLUDE

