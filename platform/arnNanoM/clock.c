/*
 * Copyright (c) 2005, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: clock.c,v 1.3 2009/09/09 21:11:24 adamdunkels Exp $
 */

/**
 * \file
 *         Clock implementation for Unix.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "36064s.h"
#include "sys/clock.h"
#include <time.h>
#include <sys/time.h>
#include "dev/watchdog.h"

static volatile clock_time_t ClkCounter; 
#define RESET_SYSCLOCK_I (IRR2.BIT.IRRTB1 = 0)

void
clock_init(void)
{
//   clock di sistema
IENR2.BIT.IENTB1 = 1; // overflow interrupt is enabled
TB1.TMB1.BIT.CKS = 4; // 000: Internal clock: Phi/8192
					// 001: Internal clock: Phi/2048
					// 010: Internal clock: Phi/512
					// 011: Internal clock: Phi/256
					// 100: Internal clock: Phi/64
					// 101: Internal clock: Phi/16
					// 110: Internal clock: Phi/4
//TB1.TMB1.BIT.CKS = 2;
//TB1.TCB1 = 99; // e' in modo autoreload; cosi' conta da 99 a 256 con Phi / 512; il periodo diventa 5 ms
TB1.TCB1 = 5; // e' in modo autoreload; cosi' conta da 6 a 256 con Phi / 64; il periodo diventa 1 ms
TB1.TMB1.BIT.RLD = 1;

ClkCounter = 0;  
}

/*---------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{
  return ClkCounter;
}
/*---------------------------------------------------------------------------*/
unsigned long
clock_seconds(void)
{
  return (ClkCounter / 1000);
}
/*---------------------------------------------------------------------------*/
void
clock_delay(unsigned int d)
{
  static clock_time_t tmp;
  tmp = ClkCounter;
  while ((ClkCounter - tmp) < d) {;} 
}
/*---------------------------------------------------------------------------*/


void _TB1_TMIB1_ (void)
{
RESET_SYSCLOCK_I;
ClkCounter++;
//BEGIN reset watchdog: on h836064 stop of timer seem doesn't work !!! CHEWCK BETTER!!!
	WDT.TCWD = 0;
//END  on h836064 stop of timer seem doesn't work !!! CHEWCK BETTER!!!

}
