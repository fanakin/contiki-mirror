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
 * $Id: CommandList.c,v 1.0 2012/03/01 18:18:51 fabiogiovagnini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio Giovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-03-01
 * Updated : $Date: 2012/03/01 18:18:51 $
 *           $Revision: 1.0 $
 */

#include "string.h"


//BEGIN COMMAND IMPLEMENTATION HEADERS
#include "command_CPIN.h"
#include "command_CSQ.h"
#include "command_CSMS.h"
#include "command_CPMS.h"
#include "command_CMGF.h"
#include "command_CMGS.h"
//Add Here a new command header
//END COMMAND IMPLEMENTATION HEADERS

#include "CommandDef.h"
#include "CommandList.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

unsigned char statusCode = 0;
unsigned char commandExitCode = 0;
const char LF[] = {10,0}; // 10 decimale <LF>
const char CR[] = {13,0}; // 13 decimale <CR>
const char CTRL_Z[] = {26,0}; // 26 decimale <CTRL+Z>

static void* command_Help(void* cmd, void* data);

#define COMMAND_LIST_VERSION "Command List 0.1\r\n"
#define COMMAND_GENERAL_DESCR "Tutti i comandi accettano l'opzione ? e =?. \
ES. +CPIN? oppure +CPIN=?\r\n\
HELP <command><CR>\r\n\
restitutisce l'HELP del singolo comando.\r\n\
Es. HELP +CPIN<CR>\r\n\
\r\n"
const arnGsmRemoteCommand_t CommandList[] = {
/*  0*/  {"HELP",COMMAND_LIST_VERSION COMMAND_GENERAL_DESCR,command_Help,NULL},
/*  1*/  {"+CPIN",command_CPIN_HELP_IT,command_CPIN,response_CPIN},
/*  2*/  {"+CSQ",command_CSQ_HELP_IT,command_CSQ,response_CSQ},
/*  3*/  {"+CSMS",command_CSMS_HELP_IT,command_CSMS,response_CSMS},
/*  4*/  {"+CPMS",command_CPMS_HELP_IT,command_CPMS,response_CPMS},
/*  5*/  {"+CMGF",command_CMGF_HELP_IT,command_CMGF,response_CMGF},
/*  6*/  {"+CMGS",command_CMGS_HELP_IT,command_CMGS,response_CMGS},
/*   */  {NULL,NULL,NULL,NULL}
};

arnGsmRemoteCommand_t* arnCommand(const char* CMD)
{
  unsigned short i;
  arnGsmRemoteCommand_t* pCmd;

  for (i=0,pCmd = (arnGsmRemoteCommand_t*)CommandList; pCmd->Command != NULL; ) {
    if (!strncmp(CMD,pCmd->Command,strlen(pCmd->Command))) { // command found
      return pCmd;
    }
    pCmd = (arnGsmRemoteCommand_t*)(&(CommandList[++i]));
  }
  return NULL;
}

char* arnCommandParameters(const arnGsmRemoteCommand_t* foundCommand, char* CMD)
{
    CMD += strlen(foundCommand->Command);
    if (CMD && (*CMD)) return CMD;
    return NULL;
}

void* command_Help(void* cmd, void* data)
{
  int i;
  arnGsmRemoteCommand_t* pCmd;
  struct timer tmr;
  
  if (data == NULL) {
    printf("%s\n\r",CommandList[0].HelpString);
    for (i = 1, pCmd = (arnGsmRemoteCommand_t*)(&CommandList[1]); pCmd->Command != NULL; ) {
      printf("%s:%s\n\r",CommandList[i].Command,CommandList[i].HelpString);
      pCmd = (arnGsmRemoteCommand_t*)(&(CommandList[++i]));
      timer_set(&tmr,20);
      while (!timer_expired(&tmr));
    }
  return NULL;
  }
  else {
    char* Dt = data;
    Dt++; // HELP <command>
    for (i = 1, pCmd = (arnGsmRemoteCommand_t*)(&CommandList[1]); pCmd->Command != NULL; ) {
      if (!strncmp(CommandList[i].Command,Dt,strlen(CommandList[i].Command))) {
	printf("%s:%s\n\r",CommandList[i].Command,CommandList[i].HelpString);
	return NULL;
      }
      pCmd = (arnGsmRemoteCommand_t*)(&(CommandList[++i]));
    }
    printf("Comando %s non trovato\n\r",Dt);
    return NULL;
  }
}
