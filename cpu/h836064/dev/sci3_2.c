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
#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

static int (*sci3_2_input_handler)(unsigned char c);
static volatile uint8_t transmitting;
#define TXBUFSIZE 32
static struct ringbuf txbuf;
static uint8_t txbuf_data[TXBUFSIZE];
#if 0
static volatile uint8_t rx_in_progress;
#endif

//#define __DEBUG__
#ifdef __DEBUG__
static unsigned char app[2];
static const unsigned char Nib_to_Hex[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46};
static void uc_to_2_cifre(unsigned char vett[2],unsigned char a1)
{
/* LOCAL VARIABLES */
/* RETURN VALUE */
/* BODY FUNCTION */
vett[0] = Nib_to_Hex[(unsigned char) ((a1 & 0xF0) >> 4)];
vett[1] = Nib_to_Hex[(unsigned char) ((a1 & 0x0F))];
/* END BODY */
}
/*static*/ void lowlevelTx(unsigned char ch)
{
  /* Loop until the transmission buffer is available. */
  while (!(SCI3_2.SSR.BIT.TDRE));

  /* Transmit the data. */
  SCI3_2.TDR = ch;
  SCI3_2.SSR.BIT.TDRE = 0;
}

#endif

static unsigned char baudrateToBRR(unsigned long baud)
{
  unsigned char res;
  switch (baud) {
    case 115200: res = 3; break;
    case 57600: res = 8; break;
    case 38400: res = 12; break;
    case 19200: res = 25; break;
    case 9600: res = 51; break;
    default: res = 51; break;
  }
  return res;
}

/*---------------------------------------------------------------------------*/
unsigned char
sci3_2_active(void)
{
  return (transmitting);
}
/*---------------------------------------------------------------------------*/
void
sci3_2_set_input(int (*input)(unsigned char c))
{
  sci3_2_input_handler = input;
}
/*---------------------------------------------------------------------------*/
void
sci3_2_writeb(unsigned char c)
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

    SCI3_2.SCR3.BYTE |= 0x04; /*  */
    SCI3_2.SSR.BYTE &= ~0x04; /* reset TxEI request */
  }
#else /* TX_WITH_INTERRUPT */
  /* Loop until the transmission buffer is available. */
  while (!(SCI3_2.SSR.BIT.TDRE));

  /* Transmit the data. */
  SCI3_2.TDR = c;
  SCI3_2.SSR.BIT.TDRE = 0;
#endif /* TX_WITH_INTERRUPT */
}
/*---------------------------------------------------------------------------*/
/**
 * Initalize the RS232 port.
 *
 */
void
sci3_2_init(unsigned long ubr)
{

ringbuf_init(&txbuf, txbuf_data, sizeof(txbuf_data));
  
 
MSTCR2.BYTE &= ~0x80; /* stop mode sci3_2 cleared */

IO.PMR1.BIT.TXD2 = 1; /* TXD_2 = 1 */

SCI3_2.SCR3.BYTE &= ~0x30; /*TE and  RE  disabled*/

SCI3_2.SMR.BYTE |= (0x00 & 0x03); /*asynch. mode, 8,N,1 Clock source fixed by n value */
SCI3_2.BRR = baudrateToBRR(ubr); /* with SMR it selects baud rate */
SCI3_2.SCR3.BYTE &= ~0x03; /* internal clock, asynchronus mode  */
SCI3_2.SCR3.BYTE |= 0x30; /*TE and  RE  enabled*/

SCI3_2.SCR3.BYTE |= 0x44; /*RxI Enabled Teie enabled*/
}


/*---------------------------------------------------------------------------*/
//Interrupt handler
void _SCI3_2_(void)
{
if (SCI3_2.SSR.BIT.RDRF) {
  SCI3_2.SSR.BIT.RDRF = 0;
  reset_imask_ccr();
#ifdef __DEBUG__
  printf("%2x",SCI3_2.RDR);
#endif
  if(sci3_2_input_handler != NULL) {
    if(sci3_2_input_handler(SCI3_2.RDR)) {;}
  }
}
else if (SCI3_2.SSR.BIT.OER || SCI3_2.SSR.BIT.FER || SCI3_2.SSR.BIT.PER) {
  SCI3_2.SCR3.BYTE &= ~0x40;
  SCI3_2.SCR3.BYTE &= ~0x10;
  SCI3_2.SSR.BYTE &= ~0x78;
  SCI3_2.SCR3.BYTE |= 0x10;
  SCI3_2.SCR3.BYTE |= 0x40;
  reset_imask_ccr();
}
/*else if () {
	}*/
else if (SCI3_2.SSR.BIT.TEND) {
	if(ringbuf_elements(&txbuf) == 0) {
		transmitting = 0;
		SCI3_2.SCR3.BYTE &= ~0x04;
	} else {
		SCI3_2.TDR = ringbuf_get(&txbuf);
		}
	}
}
/*---------------------------------------------------------------------------*/
