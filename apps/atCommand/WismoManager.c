/*
 * Copyright (c) 2012, Aurion s.r.l. (Bologna Italy)
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
 * $Id: WismoManager.c,v 0.1 2012/02/13 18:18:51 fabiogiovagnini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio Giovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-02-13
 * Updated : $Date: 2012/02/13 18:18:51 $
 *           $Revision: 0.1 $
 */

#include <string.h>

#include "contiki.h"
#include "dev/serial-line.h"
#include "dev/eeprom.h"
#include "../../platform/arnNanoM/dev/wismo218-arch.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

#include "CommandManager.h"
#include "WismoManager.h"
#include "CommandDef.h"

process_event_t wismo218_status_event;
process_event_t wismo218_data_event;
static struct etimer timer;
static wismo218_status_t Status;

static arnGsmRemoteCommand_t* Command;
static arnGsmRemoteResponse_t Answer;


PROCESS(wismomanager_process, "WismoManager");
//AUTOSTART_PROCESSES(&wismomanager_process);

PROCESS_THREAD(wismomanager_process, ev, data)
{
  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();

  printf("WismoManager started.\n\r");
  etimer_set(&timer, 2 * CLOCK_SECOND);
  Status = init_start;
  wismo218_status_event = process_alloc_event();
  wismo218_data_event = process_alloc_event();
  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == PROCESS_EVENT_TIMER) {
      switch (Status) {
	case init_start: printf("Wismo reset start...\r\n"); Status = init_w_of; etimer_set(&timer, 2 * CLOCK_SECOND); break;
	case init_w_of: printf("off\r\n"); Status = init_w_on; wismo218_On(); etimer_set(&timer, 2 * CLOCK_SECOND); break;
	case init_w_on: printf("on\r\n"); Status = init_w_reset; wismo218_Reset(); etimer_set(&timer, 2 * CLOCK_SECOND); break;
	case init_w_reset: Status = init_done; break;
	default: break; 
	/* Broadcast event */
      }
      process_post(PROCESS_BROADCAST, wismo218_status_event, &Status); 
      /* Wait until all processes have handled the wismo218_status_event event */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
    else if (ev == wismo218_command_event) {
      if (data != NULL) {
	Command = data;
	/* Wait until all processes have handled the wismo218_data_event line event */
	if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
	}
      }
    }
    else if ((ev == wismo128_event_message) ||
      (ev == wismo128_gt_message)
    ) {
      if (data != NULL)  {
	char* Dt = data;
	// Parse the answer of wismo
	if (Command) {
	  unsigned char* pres;
	  if (Command->Response_handler) {
	    pres = Command->Response_handler(Command,Dt,&Answer);
	    if (pres) {
	      //printf("pres non NULL\r\n");
	      if (*pres == 1) process_post(PROCESS_BROADCAST, wismo218_data_event, &Answer);
	    }
	  }
	}
	else {/*printf("Unsolicited Command:%s\r\n",Dt);*/}
      }
      /* Wait until all processes have handled the wismo218_data_event line event */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
  }

 exit:
  printf("Exit WismoManager\n\r");
  PROCESS_END();
}


void
WismoManager_Init(void)
{
  process_start(&wismomanager_process, NULL);
}
