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
 * Author  : Fabio Giovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-02-13
 * Updated : $Date: 2012/02/13 18:18:51 $
 *           $Revision: 1.0 $
 */

#if 0
#include "contiki.h"
#include "dev/wismo218.h"
#include "dev/serial-line.h"
#include <string.h>

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif
#define MAX_CMD_PARAMS_ALLOWED_LEN	32
static const char* CommandString[] = {
  "+CSMS",
  "+CSTA",
  "+CPIN",
  "+CSQ",
  "+CIND",
  "+CGMI",
  "+CGMM",
  "+CGMR",
  "+CGSN",
  "+CSMS",
  "+CPMS",
  "+CMGF",
  "+CSCA",
  "+CSCB",
  "+CSMP",
  "+CSDH",
  "+CSAS",
  "+CRES",
  "+CMGS",
  "+CNMI",
  "+CMGL",
  "WSMON",
  "WSMOF",
  "WSMRST",
  NULL
};
static const char TestMessage[] = {
  'P',
  'R',
  'O',
  'V',
  'A',
  0x1a,
};
static int cmdParser(char* ln);

process_event_t wismo218_command_event;
static wismo218Cmd_t Wismo218Command;

PROCESS(commandmanager_process, "CommandManager");
//AUTOSTART_PROCESSES(&commandmanager_process);

PROCESS_THREAD(commandmanager_process, ev, data)
{
  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();
  
 wismo218_command_event = process_alloc_event();
  
  printf("CommandManager started.\n\r");
  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == serial_line_event_message) {
      if (data != NULL) {
	char* Dt = data;
	//printf("%s:%s\n\r",__FUNCTION__,Dt);
	if (!cmdParser(Dt)) {
	  /* Broadcast event */
	  process_post(PROCESS_BROADCAST, wismo218_command_event, &Wismo218Command);
	  
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
  Wismo218Command.Cmd = NULL;
  Wismo218Command.Params = NULL;
  for (i = 0,pCmd = ((char*)CommandString[0]); pCmd != NULL; ) {
    if (!strncmp(ln,pCmd,strlen(pCmd))) { // command found
      Wismo218Command.Cmd = pCmd;
      ptr += strlen(pCmd);  
      break;
    }
    pCmd = (char*)CommandString[++i];
  }
  if (pCmd == NULL) return -1;
  else if (strlen((const char*)&ln[ptr]) < MAX_CMD_PARAMS_ALLOWED_LEN) {
    Wismo218Command.Params = (&(ln[ptr]));
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

#endif

#if 1
#include <string.h>

#include "contiki.h"
#include "dev/wismo218.h"
#include "dev/serial-line.h"
#include "CommandList.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

static arnGsmRemoteCommandGruop_t cmdParser(char* ln);

process_event_t wismo218_command_event;
static wismo218Cmd_t Wismo218Command = {"","","",0x03};

const char SUFFIX[] = {13,0}; // 13 decimale <CR>
const char CTRL_Z[] = {26,0}; // 26 decimale <CTRL+Z>


PROCESS(commandmanager_process, "CommandManager");
//AUTOSTART_PROCESSES(&commandmanager_process);

PROCESS_THREAD(commandmanager_process, ev, data)
{
  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();
  
  wismo218_command_event = process_alloc_event();
  
  printf("CommandManager started.\n\r");
  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == serial_line_event_message) {
      if (data != NULL) {
	char* Dt = data;
	arnGsmRemoteCommandGruop_t foundgrp = cmdParser(Dt);
	//printf("%s:%s\n\r",__FUNCTION__,Dt);
	if ((foundgrp == aGRCG_wismo218) ||
	  (foundgrp == aGRCG_wismo218_CTRLZ)
	) {
	  /* Broadcast event */
	  process_post(PROCESS_BROADCAST, wismo218_command_event, &Wismo218Command);
	}
	else if (cmdParser(Dt) == aGRCG_unknown) {
	  printf("Unsupported Command\n\r");
	}
	else if (cmdParser(Dt) == aGRCG_generic) {}
	else {
	  printf("Unimplemented Command\n\r");
	}
      }
      /* Wait until all processes have handled the serial line event */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
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

arnGsmRemoteCommandGruop_t cmdParser(char* ln)
{
  int i;
  int ptr = 0;
  arnGsmRemoteCommand_t* pCmd;
  arnGsmRemoteCommandGruop_t foundGroup;

  Wismo218Command.Cmd = NULL;
  Wismo218Command.Params = NULL;
  Wismo218Command.ActivationFlags = 0;
  
  for (i = 0,pCmd = (arnGsmRemoteCommand_t*)CommandList; pCmd != NULL; ) {
    if (!strncmp(ln,pCmd->CommandString,strlen(pCmd->CommandString))) { // command found
      Wismo218Command.Cmd = (char*)(pCmd->CommandString);
      ptr += strlen(pCmd->CommandString);
      foundGroup = pCmd->Gruop;
      break;
    }
    pCmd = (arnGsmRemoteCommand_t*)(&(CommandList[++i]));
  }
  if (pCmd == NULL) return aGRCG_unknown;
  switch (foundGroup) {
    case aGRCG_generic: pCmd->handler(NULL); return foundGroup;
    case aGRCG_eeprom: break;
    case aGRCG_wismo218:
      if (strlen((const char*)&ln[ptr]) < MAX_CMD_PARAMS_ALLOWED_LEN) {
	Wismo218Command.subParams1 = 0;
	Wismo218Command.Params = (&(ln[ptr]));
	strcat(Wismo218Command.Params,SUFFIX);
	Wismo218Command.ActivationFlags |= 0x03;
	return foundGroup;
      }
      break;
    case aGRCG_wismo218_CTRLZ:
      if (strlen((const char*)&ln[ptr]) < MAX_CMD_PARAMS_ALLOWED_LEN) {
	char* p;
	Wismo218Command.Params = (&(ln[ptr]));
	p = strchr(Wismo218Command.Params,' ');
	if (p) {
	  *p = SUFFIX[0];
	  p++; *p = 0; p++;
	  Wismo218Command.subParams1 = p;
	  strcat(Wismo218Command.subParams1,CTRL_Z);
	  Wismo218Command.ActivationFlags |= 0x07;
	  return foundGroup;
	}
      }
      break;
    default: break;
  }
  return aGRCG_none;
}


void printHelp(void* p)
{
  int i;
  arnGsmRemoteCommand_t* pCmd;
  struct timer tmr;
  
  printf("%s\n\r",CommandList[0].HelpString);
  for (i = 1, pCmd = (arnGsmRemoteCommand_t*)(&CommandList[1]); pCmd->CommandString != NULL; ) {
    printf("%s:%s\n\r",CommandList[i].CommandString,CommandList[i].HelpString);
    pCmd = (arnGsmRemoteCommand_t*)(&(CommandList[++i]));
    timer_set(&tmr,100);
    while (!timer_expired(&tmr)) ;
  }
}
#endif

