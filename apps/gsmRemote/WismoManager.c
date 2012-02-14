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
#include "dev/leds.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

#include "CommandManager.h"


static wismo218Ans_t Answer;
static void parserAns(char* Data, wismo218Ans_t* Ans);
static void parserCmdParams(wismo218Cmd_t* Cmd,char* bff);


PROCESS(wismomanager_process, "WismoManager");

//AUTOSTART_PROCESSES(&wismomanager_process);

PROCESS_THREAD(wismomanager_process, ev, data)
{
  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();

  printf("WismoManager started.\n\r");
  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == command_event) {
      if (data != NULL) {
	wismo218Cmd_t* Dt = data;
	// send command to wismo
	printf("%s:%s\n\r","Command",Dt->Cmd);
	printf("%s:%s\n\r","Params",Dt->Params);
	parserCmdParams(Dt,NULL);
      }
    }
    else if (ev == wismo128_event_message) {
	char* Dt = data;
      // Parse the answer of wismo
      parserAns(Dt,&Answer);
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
  return;
}

void
parserCmdParams(wismo218Cmd_t* Cmd,char* bff)
{
  if (!strcmp(Cmd->Cmd,"READ")) {
    unsigned char bff[16];
    unsigned char i;
    eeprom_read(0,bff,sizeof(bff));
    for (i = 0; i < sizeof(bff); i++) {
      printf("0x%2x\r\n",bff[i]);
    }
  }
  else if (!strcmp(Cmd->Cmd,"WRITE")) {
    unsigned char bff[16] = {20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,36};
    eeprom_write(0,bff,sizeof(bff));
    printf("eeprom write done.\r\n");
  }
  else if (!strcmp(Cmd->Cmd,"LGON")) {
    leds_on(LEDS_GREEN);
  }
  else if (!strcmp(Cmd->Cmd,"LGOF")) {
    leds_off(LEDS_GREEN);
  }
  else if (!strcmp(Cmd->Cmd,"LYON")) {
    leds_on(LEDS_YELLOW);
  }
  else if (!strcmp(Cmd->Cmd,"LYOF")) {
    leds_off(LEDS_YELLOW);
  }
  else if (!strcmp(Cmd->Cmd,"LRON")) {
    leds_on(LEDS_RED);
  }
  else if (!strcmp(Cmd->Cmd,"LROF")) {
    leds_off(LEDS_RED);
  }
}