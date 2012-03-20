/*
 * Copyright (c) 2012, Aurion s.r.l. (Bologna - Italy)
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
 * $Id: eeprom.c,v 0.1 2012/02/14 21:11:24 fabiogiovagnini Exp $
 */

/**
 * \file
 *         wismo218 low level hadrware driver
 * \author
 *         Fabio Giovagnini <fabio.giovagnini@aurion-tech.com>
 */

#include "wismo218-arch.h"
#include "sys/timer.h"

#define ON_NOT_OFF	IO.PDR1.BIT.B0
#define NOT_RESET	IO.PDR1.BIT.B2
#define READY		IO.PDR1.BIT.B6

void 
wismo218_Init(void)
{
	wismo218_On();
}

void
wismo218_On(void)
{
	ON_NOT_OFF = 1;
}

void
wismo218_Off(void)
{
	ON_NOT_OFF = 0;
}

void 
wismo218_Reset(void)
{
	struct timer tmr;
	NOT_RESET = 0;
	timer_set(&tmr,100); // deve essere almeno 38 ms
	while (!timer_expired(&tmr)) ;
	NOT_RESET = 1;

}

int
is_wismo218_Ready(void)
{
	return READY;
}
