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
 * $Id: leds-arch.c,v 1.1 2012/02/14 22:41:31 fabiogiovagnini Exp $
 */

/**
 * \file
 *         A brief description of what this file is.
 * \author
 *         Fabio Giovagnini <fabio.giovagnini@auiron-tech.com>
 */

#include "36064s.h"
#include "dev/leds.h"

#define LED0				IO.PDR6.BIT.B0
#define LED0_ON				LED0 = 0
#define LED0_OF				LED0 = 1

#define LED1				IO.PDR6.BIT.B1
#define LED1_ON				LED1 = 0
#define LED1_OF				LED1 = 1

#define LED2				IO.PDR6.BIT.B2
#define LED2_ON				LED2 = 0
#define LED2_OF				LED2 = 1

static unsigned char leds;
/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
  leds = 0;
  LED0_OF;
  LED1_OF;
  LED2_OF;
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  return leds;
}
/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char l)
{
  if (l & LEDS_GREEN) {LED0_ON; leds |= LEDS_GREEN;}
  else {LED0_OF; leds &= ~LEDS_GREEN;}
  if (l & LEDS_YELLOW) {LED1_ON; leds |= LEDS_YELLOW;}
  else {LED1_OF; leds &= ~LEDS_YELLOW;}
  if (l & LEDS_RED) {LED2_ON; leds |= LEDS_RED;}
  else {LED2_OF; leds &= ~LEDS_RED;}
}
/*---------------------------------------------------------------------------*/
