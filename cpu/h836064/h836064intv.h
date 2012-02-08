/*! \file 	h8sintv.h
*   \brief 	Definizioni del vettore degli interrupt
*
*
*/



#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _H836064INTV_H
unsigned char stckarr[252] __attribute__((section(".stack")));
extern void start (void); /* e' l'entry point definito in ctrO*/



void _RESERVED (void);


void _NMI_HANDLER (void);			/* 7 */
void _TRAP0_HANDLER (void);			/* 8 */
void _TRAP1_HANDLER (void);			/* 9 */
void _TRAP2_HANDLER (void);			/* 10 */
void _TRAP3_HANDLER (void);			/* 11 */
void _ADDRESSBREAK_HANDLER (void);	/* 12 */
void _SLEEP_HANDLER (void);			/* 13 */
void _IRQ0_HANDLER (void);			/* 14 */
void _IRQ1_HANDLER (void);			/* 15 */
void _IRQ2_HANDLER (void);			/* 16 */
void _IRQ3_HANDLER (void);			/* 17 */
void _WKP_HANDLER (void);			/* 18 */
void _TVI_HANDLER (void);			/* 22 */
void _SCI3_HANDLER (void);			/* 23 */
void _IIC2_HANDLER (void);			/* 24 */
void _ADI_HANDLER (void);			/* 25 */
void _TZITMZ0_HANDLER (void);		/* 26 */
void _TZITMZ1_HANDLER (void);		/* 27 */
void _TB1TMIB1_HANDLER (void);		/* 29 */
void _SCI3_2_HANDLER (void);		/* 32 */

#else

#endif

#ifdef __cplusplus
}
#endif
