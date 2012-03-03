/*! \file 	h8sintv.h
*   \brief 	Definizioni del vettore degli interrupt
*
*
*/


#include "int_hndl.h"

#define  _H836064INTV_H
#include "h836064intv.h"


#ifdef __cplusplus
extern "C" {
#endif

__asm__(
".macro BEGIN_ISR\n"
"push.l er0\n"
"push.l er1\n"
"push.l er2\n"
"push.l er3\n"
"push.l er4\n"
"push.l er5\n"
"push.l er6\n"
".endm\n"
);

__asm__(
".macro END_ISR\n"
"pop.l er6\n"
"pop.l er5\n"
"pop.l er4\n"
"pop.l er3\n"
"pop.l er2\n"
"pop.l er1\n"
"pop.l er0\n"
"rte\n"
".endm\n"
);

/*
__asm__(
".macro BEGIN_ISR\n"
"push.w r0\n"
"push.w e0\n"
"push.w r1\n"
"push.w e1\n"
"push.w r2\n"
"push.w e2\n"
"push.w r3\n"
"push.w e3\n"
"push.w r4\n"
"push.w e4\n"
"push.w r5\n"
"push.w e5\n"
"push.w r6\n"
"push.w e5\n"
".endm\n"
);

__asm__(
".macro END_ISR\n"
"pop.w e6\n"
"pop.w r6\n"
"pop.w e5\n"
"pop.w r5\n"
"pop.w e4\n"
"pop.w r4\n"
"pop.w e3\n"
"pop.w r3\n"
"pop.w e2\n"
"pop.w r2\n"
"pop.w e1\n"
"pop.w r1\n"
"pop.w e0\n"
"pop.w r0\n"
"rte\n"
".endm\n"
);
*/

__asm__(
".section .text\n"
".align 1\n"
".global __RESERVED\n"
"__RESERVED:\n"
"loop:\n"
"bra loop\n"
);


/* ************************** */
/* ********* NMI ************ */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __NMI_HANDLER\n"
"__NMI_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__NMI_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* TRAP0 ********** */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __TRAP0_HANDLER\n"
"__TRAP0_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__TRAP0_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* TRAP1 ********** */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __TRAP1_HANDLER\n"
"__TRAP1_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__TRAP1_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* TRAP2 ********** */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __TRAP2_HANDLER\n"
"__TRAP2_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__TRAP2_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* TRAP3 ********** */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __TRAP3_HANDLER\n"
"__TRAP3_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__TRAP3_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* ADDRESSBREAK *** */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __ADDRESSBREAK_HANDLER\n"
"__ADDRESSBREAK_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__ADDRESSBREAK_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* SLEEP ********** */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __SLEEP_HANDLER\n"
"__SLEEP_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__SLEEP_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* IRQ ************ */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __IRQ0_HANDLER\n"
"__IRQ0_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__IRQ0_\n"
"END_ISR\n"
);
__asm__(
".section .text\n"
".align 1\n"
".global __IRQ1_HANDLER\n"
"__IRQ1_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__IRQ1_\n"
"END_ISR\n"
);
__asm__(
".section .text\n"
".align 1\n"
".global __IRQ2_HANDLER\n"
"__IRQ2_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__IRQ2_\n"
"END_ISR\n"
);
__asm__(
".section .text\n"
".align 1\n"
".global __IRQ3_HANDLER\n"
"__IRQ3_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__IRQ3_\n"
"END_ISR\n"
);
__asm__(
".section .text\n"
".align 1\n"
".global __WKP_HANDLER\n"
"__WKP_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__WKP_\n"
"END_ISR\n"
);

/* ************************** */
/* ******** TV_I ************ */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __TVI_HANDLER\n"
"__TVI_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__TV_I_\n"
"END_ISR\n"
);

/* ************************** */
/* ******** ADI ************* */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __ADI_HANDLER\n"
"__ADI_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__ADI_\n"
"END_ISR\n"
);


/* ************************** */
/* ********* SCI3 *********** */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __SCI3_HANDLER\n"
"__SCI3_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__SCI3_\n"
"END_ISR\n"
);
// My debugging tools

/* ************************** */
/* ********* IIC2 *********** */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __IIC2_HANDLER\n"
"__IIC2_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__IIC2_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* TZ_ITMZ0 ******* */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __TZITMZ0_HANDLER\n"
"__TZITMZ0_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__TZ_ITMZ0_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* TZ_ITMZ1 ******* */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __TZITMZ1_HANDLER\n"
"__TZITMZ1_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__TZ_ITMZ1_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* TB1_TMIB1 ****** */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __TB1TMIB1_HANDLER\n"
"__TB1TMIB1_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__TB1_TMIB1_\n"
"END_ISR\n"
);

/* ************************** */
/* ********* SCI3_2 ********* */
/* ************************** */
__asm__(
".section .text\n"
".align 1\n"
".global __SCI3_2_HANDLER\n"
"__SCI3_2_HANDLER :\n"
"BEGIN_ISR\n"
"jsr @__SCI3_2_\n"
"END_ISR\n"
);

#ifdef __cplusplus
};

#endif

