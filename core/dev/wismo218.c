/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 * @(#)$Id: leds.c,v 1.7 2009/02/24 21:30:20 adamdunkels Exp $
 */

#include "string.h"
#include "dev/sci3_2-putchar.h"
#include "dev/wismo218-arch.h"
#include "dev/wismo218.h"
#include "sys/clock.h"
#include "sys/energest.h"
#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

#include "lib/ringbuf.h"

#ifdef WISMO128_LINE_CONF_BUFSIZE
#define BUFSIZE WISMO128_LINE_CONF_BUFSIZE
#else /* SERIAL_LINE_CONF_BUFSIZE */
#define BUFSIZE 128
#endif /* SERIAL_LINE_CONF_BUFSIZE */

#if (BUFSIZE & (BUFSIZE - 1)) != 0
#error SERIAL_LINE_CONF_BUFSIZE must be a power of two (i.e., 1, 2, 4, 8, 16, 32, 64, ...).
#error Change SERIAL_LINE_CONF_BUFSIZE in contiki-conf.h.
#endif

#if CONTIKI_TARGET_ARNNANOM
// minicom --noinit starts with 0x0d as char sent by carriage return
#define IGNORE_CHAR(c) (c == 0x0d)
#define END 0x0a
#else
#endif

#define GREATHER_THEN '>'

static const char AT[] = "AT";
static struct ringbuf rxbuf;
static uint8_t rxbuf_data[BUFSIZE];


PROCESS(wismo128_ans_process, "Wisom128 driver");

process_event_t wismo128_event_message;
process_event_t wismo128_gt_message;

/*---------------------------------------------------------------------------*/
int
wismo128_line_input_byte(unsigned char c)
{
  static uint8_t overflow = 0; /* Buffer overflow: ignore until END */
  //printf("%c",c);
  if(IGNORE_CHAR(c)) {
    return 0;
  }

  if(!overflow) {
    /* Add character */
    if(ringbuf_put(&rxbuf, c) == 0) {
      /* Buffer overflow: ignore the rest of the line */
      overflow = 1;
    }
  } else {
    /* Buffer overflowed:
     * Only (try to) add terminator characters, otherwise skip */
    if(c == END && ringbuf_put(&rxbuf, c) != 0) {
      overflow = 0;
    }
  }

  /* Wake up consumer process */
  process_poll(&wismo128_ans_process);
  return 1;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(wismo128_ans_process, ev, data)
{
  static char buf[BUFSIZE];
  static int ptr;

  PROCESS_BEGIN();

  wismo128_event_message = process_alloc_event();
  wismo128_gt_message =  process_alloc_event();
  ptr = 0;

  while(1) {
    /* Fill application buffer until newline or empty */
    int c = ringbuf_get(&rxbuf);
    if(c == -1) {
      /* Buffer empty, wait for poll */
      PROCESS_YIELD();
    } else {
      //printf("%c",c);
      if(c != END) {
        if(ptr < BUFSIZE-1) {
          buf[ptr++] = (uint8_t)c;
	  if (c == GREATHER_THEN) {
	    /* Broadcast event */
	    process_post(PROCESS_BROADCAST, wismo128_gt_message, buf);
	    /* Wait until all processes have handled the serial line event */
	    if(PROCESS_ERR_OK ==
	      process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
	    }
	    ptr = 0;
	  }
	} else {
          /* Ignore character (wait for EOL) */
        }
      } else {
        /* Terminate */
        buf[ptr++] = (uint8_t)'\0';

        /* Broadcast event */
        process_post(PROCESS_BROADCAST, wismo128_event_message, buf);

        /* Wait until all processes have handled the serial line event */
        if(PROCESS_ERR_OK ==
          process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
          PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
        }
        ptr = 0;
      }
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
void
wismo218_init(void)
{
  ringbuf_init(&rxbuf, rxbuf_data, sizeof(rxbuf_data));
  process_start(&wismo128_ans_process, NULL);
  
}
/*---------------------------------------------------------------------------*/

int wismo218_sendCommand(const char* Cmd)
{
  int i;
  if (sci3_2_putchar(AT[0]) <= 0) return -1;
  if (sci3_2_putchar(AT[1]) <= 0) return -2;
  for ( i = 0; i < strlen(Cmd); i++) {
    if (sci3_2_putchar(Cmd[i]) <= 0) return -3;
  }
  return 0;
}

int wismo218_sendParams(const char* Params)
{
  int i;
  for ( i = 0; i < strlen(Params); i++) {
    if (sci3_2_putchar(Params[i]) <= 0) return -3;
  }
  return 0;
}

