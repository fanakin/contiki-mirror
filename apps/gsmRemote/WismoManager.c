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

#include "WismoManager.h"
#include "CommandManager.h"

process_event_t wismo218_status_event;
process_event_t wismo218_data_event;
static struct etimer timer;
static wismo218_status_t Status = init_start;

static wismo218Ans_t Answer;
static wismo218Cmd_t* Command;
static void parserAns(char* Data, wismo218Ans_t* Ans);
static void parserCmdParams(wismo218Cmd_t* Cmd,char* bff);


PROCESS(wismomanager_process, "WismoManager");

//AUTOSTART_PROCESSES(&wismomanager_process);

PROCESS_THREAD(wismomanager_process, ev, data)
{
  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();

  printf("WismoManager started.\n\r");
  etimer_set(&timer, CLOCK_SECOND / 2);
  wismo218_status_event = process_alloc_event();
  wismo218_data_event = process_alloc_event();
  
  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == PROCESS_EVENT_TIMER) {
      switch (Status) {
	case init_start: Status = init_w_of; wismo218_Off(); etimer_set(&timer, CLOCK_SECOND / 2); break;
	case init_w_of: Status = init_w_on; wismo218_On(); etimer_set(&timer, CLOCK_SECOND / 2); break;
	case init_w_on: Status = init_w_reset; wismo218_Reset(); etimer_set(&timer, CLOCK_SECOND / 2); break;
	case init_w_reset: Status = init_done; break;
	default: break; 
	/* Broadcast event */
      }
    process_post(PROCESS_BROADCAST, wismo218_status_event, &Status); 
    }
    else if (ev == wismo218_command_event) {
      if (data != NULL) {
	Command = data;
	// send command to wismo
	printf("%s:%s\n\r","Command",(Command->Cmd != 0) ? Command->Cmd : "");
	printf("%s:%s\n\r","Params",(Command->Params != 0) ? Command->Params : "");
	printf("%s:%s\n\r","subParams1",(Command->subParams1 != 0) ? Command->subParams1 : "");
	parserCmdParams(Command,NULL);
      }
    }
    else if ((ev == wismo128_event_message) ||
      (ev == wismo128_gt_message)
    ) {
      char* Dt = data;
      // Parse the answer of wismo
      parserAns(Dt,&Answer);
      process_post(PROCESS_BROADCAST, wismo218_data_event, &Answer); 
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


void
parserAns(char* Data, wismo218Ans_t* Ans)
{
  printf("%s\r\n",Data);
  if (Data[0] == '>') {
    if (Command->ActivationFlags & 0x04) {if (Command->subParams1) {if (wismo218_sendParams(Command->subParams1) < 0) printf("%s - %s: error\r\n",Command->Cmd,Command->subParams1);}}
  }
  return;
}

void
parserCmdParams(wismo218Cmd_t* Cmd,char* bff)
{
  if (!strncmp(Cmd->Cmd,"+",1)) {
    printf("%s\n\r",Cmd->Cmd);
    if (Cmd->ActivationFlags & 0x01) {if (wismo218_sendCommand(Cmd->Cmd) < 0) printf("%s: error\r\n",Cmd->Cmd);}
    if (Cmd->ActivationFlags & 0x02) {if (wismo218_sendParams(Cmd->Params) < 0) printf("%s - %s: error\r\n",Cmd->Cmd,Cmd->Params);}
  }
  else if (!strcmp(Cmd->Cmd,"WRITE")) {
    unsigned char bff[16] = {20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,36};
    eeprom_write(0,bff,sizeof(bff));
    printf("eeprom write done.\r\n");
  }
  else if (!strcmp(Cmd->Cmd,"READ")) {
    int i;
    unsigned char bff[16];
    eeprom_read(0,bff,sizeof(bff));
    for (i = 0; i < 16; i++) printf("%d ",bff[i]);
    printf("\r\n");
  }
  else if (!strcmp(Cmd->Cmd,"WSMON")) {
    wismo218_On();
  }
  else if (!strcmp(Cmd->Cmd,"WSMOF")) {
    wismo218_Off();
  }
  else if (!strcmp(Cmd->Cmd,"WSMRST")) {
    wismo218_Reset();
  }
}