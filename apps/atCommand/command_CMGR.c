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
 * $Id: command_CMGR.c,v 1.0 2012/04/06 17:00:51 fabiogiovagnini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio Giovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-04-06
 * Updated : $Date: 2012/04/06 17:00:51 $
 *           $Revision: 1.0 $
 */
/**
 * \addtogroup ATCommand  AT Command implementation for +CMGR
 *
 * This command is managed with a generic AT Command handler and a generic
 * At Response handler.
 * See command_AT.h,c
 *
 * @{
 */

/**
 * \file
 *         Implementation of the CNMI handler of an AT command to send
 * \author
 *         Fabio Giovagnini <fabio.giovagnini@aurion-tech.com>
 * 
 */

#include <string.h>

#include "CommandDef.h"
#include "command_CMGL.h"
#include "dev/wismo218.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

/*
 * CMGR command Help String for Italian Language
 */ 
const char command_CMGR_HELP_IT[] = "<indice> legge il messaggio al posto <indice>";

#define NO_STATUS                       0
#define COMMAND_RECEIVED                1
#define HEADER_RECEIVED                 2
#define DATA_TEXT_RECEIVED              3


/*
 * CMGR command sender is a standard command sender
 */ 

/*
 * CMGR command response is a standard response handler
 */ 
/**
 * \brief      CMGR response handler
 * \param cmd  pointer to arnGsmRemoteCommand_t structure of the command
 * \param data pointer to char string being the answer of the command
 * \return     NULL if the command doesn't need to dispatch an event and mno error occurs,
 *             unsigned char* if it needs to dispatch an event or an error occurs.
 *             
 * \retval 0   An error occurs; examine data to know exactly
 * \retval 1   Not yet implemented
 *
 *             This function is the handler for getting back the 
 *             The answer form the +CMGS command
 *
 *             READY answer is considered good response
 *             Other answers ar considerated ERROR
 *             See WA_DEV_WISMO_UGD_012 004 November 3, 2011 wismo user AT Command manual
 *             pag.25 par 2.4.1
 * 
 */
void* response_CMGR(void* cmd, void* data, void* answer)
{
  //char *token;

  if (cmd == NULL) return NULL;
  if (answer == NULL) return NULL;
  if (data) {
    //arnGsmRemoteCommand_t *Command = cmd;
    arnGsmRemoteResponse_t *res = answer;
    res->type = TYPEVAL_MGR;
    char* Dt = data;
    switch (statusCode) {
      case NO_STATUS:
	//printf("---%s\r\n",Dt);
	statusCode = COMMAND_RECEIVED;
	return NULL;
      case COMMAND_RECEIVED:
	//printf("---%s\r\n",Dt);
	statusCode = HEADER_RECEIVED;
	return NULL;
      case HEADER_RECEIVED:
	//printf("---%s\r\n",Dt);
	memset(res->Param3.text,0,sizeof(res->Param3.text));
	strncpy(res->Param3.text,Dt,(strlen(Dt) <= sizeof(res->Param3.text) ? strlen(Dt) : sizeof(res->Param3.text)));
	statusCode = DATA_TEXT_RECEIVED;
	commandExitCode = 1;
	return &commandExitCode;
      case DATA_TEXT_RECEIVED:
	//printf("---%s\r\n",Dt);
	statusCode = NO_STATUS;
	return NULL;
      default : break;
    }
  }
  return NULL;
}



/** @} */