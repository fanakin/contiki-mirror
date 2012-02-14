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
 * $Id: blinker.c,v 1.3 2010/01/14 18:18:51 nifi Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio GIovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-02-13
 * Updated : $Date: 2012/02/13 18:18:51 $
 *           $Revision: 1.0 $
 */

#include "contiki.h"
#include "dev/wismo218.h"
#include "dev/serial-line.h"
#include <string.h>

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif
#define MAX_CMD_PARAMS_ALLOWED_LEN	64
static const char* CommandString[] = {
  "+CSMS",
  "READ",
  "WRITE",
  "LGON",
  "LGOF",
  "LYON",
  "LYOF",
  "LRON",
  "LROF",
  NULL
};

static int cmdParser(char* ln);

process_event_t command_event;
static wismo218Cmd_t Command;

PROCESS(commandmanager_process, "CommandManager");
//AUTOSTART_PROCESSES(&commandmanager_process);

PROCESS_THREAD(commandmanager_process, ev, data)
{
  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();
  
  command_event = process_alloc_event();
  
  printf("CommandManager started.\n\r");
  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == serial_line_event_message) {
      if (data != NULL) {
	char* Dt = data;
	//printf("%s:%s\n\r",__FUNCTION__,Dt);
	if (!cmdParser(Dt)) {
	  /* Broadcast event */
	  process_post(PROCESS_BROADCAST, command_event, &Command);
	  
	  /* Wait until all processes have handled the serial line event */
	  if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
	  }
	}
	else {
	  printf("Unsupported Command\n\r");
	}
      }
    } 
  }

 exit:
  printf("Exit CommandManager\n\r");
  PROCESS_END();
}

void
CommandManager_Init(void)
{
  process_start(&commandmanager_process, NULL);  
}

int cmdParser(char* ln)
{
  int i;
  int ptr = 0;
  char* pCmd;
  Command.Cmd = NULL;
  Command.Params = NULL;
  for (i = 0,pCmd = ((char*)CommandString[0]); pCmd != NULL; ) {
    if (!strncmp(ln,pCmd,strlen(pCmd))) { // command found
      Command.Cmd = pCmd;
      ptr += strlen(pCmd);  
      break;
    }
    pCmd = (char*)CommandString[++i];
  }
  if (pCmd == NULL) return -1;
  if (strlen((const char*)&ln[ptr]) < MAX_CMD_PARAMS_ALLOWED_LEN) {
    Command.Params = (&(ln[ptr]));
    return 0;
  }
  else return -2;
#if 0  
  if (ln[ptr] == '=') {
    if (ln[ptr+1] == '?') { // Supported configuration request
      ln[ptr+2] = 0;
      Command.Params = (&(ln[ptr]));
      return 0;
    }
    else { // che the line is null terminated
      Command.Params = (&(ln[ptr]));
      return 0;
    }
  }
  else if (ln[ptr] == '?') { // Current setting request
    Command.Params = (&(ln[ptr]));
    return 0;
  }
  else return -2;
#endif
}