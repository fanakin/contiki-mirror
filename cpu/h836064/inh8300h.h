/***************************************************************************
                          inth8s.h  -  description
                             -------------------
    begin                : Tue Jan 16 2001
    copyright            : (C) 2001 by fabio giovagnini
    email                : fgiovag@tin.it
 ***************************************************************************/
#ifndef _INH8S_H
#define _INH8S_H

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================*/
/* Insert instruction NOP					*/
/*==============================================================*/
__inline__ static void no_operation(void) {
	__asm__ ("nop");
}

/*==============================================================*/
/* Instructions regarding the CCR                               */
/*==============================================================*/
__inline__ static void reset_imask_ccr(void) {
	__asm__ __volatile__ ("andc #127,ccr");
}

__inline__ static void  set_imask_ccr (void){
	__asm__ ("orc #128,ccr");
}

__inline__ static unsigned char get_ccr (void)
{
	unsigned char res;
	__asm__ ("	sub.l	e%0,e%1" : "=r" (res) : "r" (res));
	__asm__ ("	stc ccr,%0l" : "=g" (res));
	return res;
}

__inline__ static unsigned long get_sp (void)
{
	unsigned long res = 0;
	/*__asm__ __volatile__ ("	sub.l	%0,%1" : "=r" (res) : "r" (res));*/
	__asm__ __volatile__ ("	mov.l er7,%0" : "=g" (res));
	return res;
}

#ifdef __cplusplus
};
#endif

#endif
