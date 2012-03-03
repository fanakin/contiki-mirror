/*
 * Copyright (c) 2006, Swedish Institute of Computer Science
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
 * @(#)$Id: uart1.c,v 1.24 2011/01/19 20:44:20 joxe Exp $
 */

/*
 * Machine dependent MSP430 UART1 code.
 */

#include "36064s.h"
#include "inh8300h.h"
#include "defines-arch.h"
#include <stdlib.h>
#include <signal.h>

#include <stdio.h>

#include "sys/energest.h"
#include "dev/sci3.h"
#include "dev/watchdog.h"
#include "sys/ctimer.h"
#include "lib/ringbuf.h"

static int (*sci3_input_handler)(unsigned char c);
static volatile uint8_t transmitting;
#define TXBUFSIZE 128
static struct ringbuf txbuf;
static uint8_t txbuf_data[TXBUFSIZE];
#if 0
static volatile uint8_t rx_in_progress;
#endif

static unsigned char baudrateToBRR(unsigned short baud)
{
  unsigned char res;
  switch (baud) {
    case 38400: res = 12; break;
    case 19200: res = 25; break;
    case 9600: res = 51; break;
    default: res = 51; break;
  }
  return res;
}

/*---------------------------------------------------------------------------*/
unsigned char
sci3_active(void)
{
  //return ((~ UTCTL1) & TXEPT) | rx_in_progress | transmitting;
  return 0;
}
/*---------------------------------------------------------------------------*/
void
sci3_set_input(int (*input)(unsigned char c))
{
  sci3_input_handler = input;
}
/*---------------------------------------------------------------------------*/
void
sci3_writeb(unsigned char c)
{
  watchdog_periodic();
#if 1

  /* Put the outgoing byte on the transmission buffer. If the buffer
     is full, we just keep on trying to put the byte into the buffer
     until it is possible to put it there. */
  while(ringbuf_put(&txbuf, c) == 0);

  /* If there is no transmission going, we need to start it by putting
     the first byte into the UART. */
  if(transmitting == 0) {
    transmitting = 1;

    /* Loop until the transmission buffer is available. */
    /*while((IFG2 & UTXIFG1) == 0);*/
    //TXBUF1 = ringbuf_get(&txbuf);

    SCI3.SCR3.BYTE |= 0x04; /*  */
    SCI3.SSR.BYTE &= ~0x04; /* reset TxEI request */
  }

#else /* TX_WITH_INTERRUPT */

  /* Loop until the transmission buffer is available. */
  while (!(SCI3.SSR.BIT.TDRE));

  /* Transmit the data. */
  SCI3.TDR = c;
  SCI3.SSR.BIT.TDRE = 0;
#endif /* TX_WITH_INTERRUPT */
}
/*---------------------------------------------------------------------------*/
/**
 * Initalize the RS232 port.
 *
 */
void
sci3_init(unsigned long ubr)
{

ringbuf_init(&txbuf, txbuf_data, sizeof(txbuf_data));
  
 
MSTCR1.BYTE &= ~0x20; /* stop mode sci3 cleared */

IO.PMR1.BIT.TXD = 1; /* TXD = 1 */

SCI3.SCR3.BYTE &= ~0x30; /*TE and  RE  disabled*/

SCI3.SMR.BYTE |= (0x00 & 0x03); /*asynch. mode, 8,N,1 Clock source fixed by n value */
SCI3.BRR = baudrateToBRR(ubr); /* with SMR it selects baud rate */
SCI3.SCR3.BYTE &= ~0x03; /* internal clock, asynchronus mode  */
SCI3.SCR3.BYTE |= 0x30; /*TE and  RE  enabled*/

SCI3.SCR3.BYTE |= 0x44; /*RxI Enabled Teie enabled*/
}


/*---------------------------------------------------------------------------*/
//Interrupt handler
void _SCI3_(void)
{
if (SCI3.SSR.BIT.RDRF) {
  SCI3.SSR.BIT.RDRF = 0;
  reset_imask_ccr();
  if(sci3_input_handler != NULL) {
    if(sci3_input_handler(SCI3.RDR)) {;}
  }
}
else if (SCI3.SSR.BIT.OER || SCI3.SSR.BIT.FER || SCI3.SSR.BIT.PER) {
  SCI3.SCR3.BYTE &= ~0x40;
  SCI3.SCR3.BYTE &= ~0x10;
  SCI3.SSR.BYTE &= ~0x78;
  SCI3.SCR3.BYTE |= 0x10;
  SCI3.SCR3.BYTE |= 0x40;
  reset_imask_ccr();
}
/*else if () {
	}*/
else if (SCI3.SSR.BIT.TEND) {
	if(ringbuf_elements(&txbuf) == 0) {
		transmitting = 0;
		SCI3.SCR3.BYTE &= ~0x04;
	} else {
		SCI3.TDR = ringbuf_get(&txbuf);
		}
	}
}
/*---------------------------------------------------------------------------*/
