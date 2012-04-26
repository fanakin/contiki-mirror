/*
 * Copyright (c) 2012, Aurion .s.r.l (Bologna - Italy)
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
 * @(#)$Id: adc.c,v 0.1 2012/02/03 10:28:32 fabiogiovagnini Exp $
 */

#include "contiki.h"

#include "36064s.h"
#include "adc.h"

#define NUM_OF_TOTAL_CHANNEL	8
static adcChannelHandler_t adcChannelHandler[NUM_OF_TOTAL_CHANNEL];

void initADC (void )
{
unsigned char i;
for (i = 0; i < NUM_OF_TOTAL_CHANNEL; i++) adcChannelHandler[i] = NULL;
AD.ADCSR.BIT.SCAN=1;	
AD.ADCSR.BIT.CKS=0;	
AD.ADCSR.BIT.CH=3;
AD.ADCSR.BIT.ADIE=0; // senza interrupt di fine conversione; conversione nel clock di sistema
AD.ADCSR.BIT.ADST=1;
}



void ADC_Inth(void)
{
while (!AD.ADCSR.BIT.ADF);
//if (!AD.ADCSR.BIT.ADF) return;
AD.ADCSR.BIT.ADF=0;
AD.ADCSR.BIT.ADST=0;
if ( AD.ADCSR.BIT.CH==3 )
	{
	if (adcChannelHandler[0]) adcChannelHandler[0](AD.ADDRA >>6);
	if (adcChannelHandler[1]) adcChannelHandler[1](AD.ADDRB >>6);
	if (adcChannelHandler[2]) adcChannelHandler[2](AD.ADDRC >>6);
	if (adcChannelHandler[3]) adcChannelHandler[3](AD.ADDRD >>6);
	AD.ADCSR.BIT.CH=7;	
	}
else
	{
	if (adcChannelHandler[4]) adcChannelHandler[4](AD.ADDRA >>6);
	if (adcChannelHandler[5]) adcChannelHandler[5](AD.ADDRB >>6);
	if (adcChannelHandler[6]) adcChannelHandler[6](AD.ADDRC >>6);
	if (adcChannelHandler[7]) adcChannelHandler[7](AD.ADDRD >>6);
	AD.ADCSR.BIT.CH=3;
	}
AD.ADCSR.BIT.ADST=1;
}


void setADCChannelHandle(unsigned char index, adcChannelHandler_t func)
{
  if (index < NUM_OF_TOTAL_CHANNEL) adcChannelHandler[index] = func;
}

