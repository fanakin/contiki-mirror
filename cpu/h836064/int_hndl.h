/*! \file 	int_hndl.h
    \brief 	Interrupt Handler functions

    In questo file vengono associati i vettori di interrupt da eseguire


*/

#ifndef __INTHNDL_H__
#define __INTHNDL_H__
#ifdef __cplusplus
extern "C" {
#endif

extern void _SCI3_ (void);
extern void _SCI3_2_ (void);
extern void _IIC2_ (void);
extern void _TZ_ITMZ1_ (void);
extern void _TB1_TMIB1_ (void);

#ifdef __cplusplus
};
#endif
#endif
