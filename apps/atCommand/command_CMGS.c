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
 * $Id: command_CMGS.c,v 1.0 2012/04/06 17:00:51 fabiogiovagnini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio Giovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-04-06
 * Updated : $Date: 2012/04/06 17:00:51 $
 *           $Revision: 1.0 $
 */
/**
 * \addtogroup ATCommand Generic AT Command implementation
 *
 * a generic AT command prepare a command with its parameters and send it
 * to the driver understanding. The driver understanding prepends the AT
 * strings or what the specific hardware expects and send really to the
 * device.
 *
 * @{
 */

/**
 * \file
 *         Implementation of the generic handler of an AT command to send
 * \author
 *         Fabio Giovagnini <fabio.giovagnini@aurion-tech.com>
 * 
 */
#include <string.h>

#include "CommandDef.h"
#include "command_CMGS.h"
#include "dev/wismo218.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

/*
 * CMGS command Help String for Italian Language
 */ 
const char command_CMGS_HELP_IT[] = "\"+393341234567\"\\PROVA MSG (+393341234567 numero di destinazione; PROVA MSG testo del messaggio;un solo \\ di separazione)";

#define CMGS_info_message_TOTAL_NUM              0
#define CMGS_info_message_SENDING_REPORT         1
static const char *CMGS_info_message_IT[] = {
  "Numero totale dei messaggi:",
  "Report dell'invio:",
  NULL
};

static char* Params;

#define GREATHER_THEN '>'

#define NO_STATUS                       0
#define NUMBER_TO_CALL_SENT_STATUS      1
#define TEXT_BODY_SENT_STATUS           2
#define FIRST_ACK_RECEIVED_STATUS       3
#define TEXT_BODY_RECEIVED_STATUS       4
#define TOTAL_MGS_NUM_RECIVED_STATUS    5
#define OK_ERROR_RECEIVED_STATUS        6

/**
 * \brief      CMGS Command
 * \param cmd  pointer to arnGsmRemoteCommand_t structure of the command
 * \param data pointer to char string being the string including the parameters of the command
 * \return     NULL if the command doesn't need to dispatch an event, unsigned char* if it needs to dispatch an event.
 *             The CMGS command always needs to return unsigned char* and a returned value of 1
 *             
 * \retval 0   Not yet implemented
 * \retval 1   To dispatch the wismo218_command_event
 *
 *             This function is the generc handler for sending action requests
 *             to am module managed with CMGS Command.
 *
 *             Most of the command can be managed with this generic handler,
 *             If an exception will occor a specific handler can be implemented
 * 
 */
void* command_CMGS(void* cmd, void* data)
{
  arnGsmRemoteCommand_t* Command = cmd;
  Params = data;
  char *token;
  
  if (!Command) return NULL;
  if (!Params) return NULL;

  statusCode = NO_STATUS;

  if (wismo218_sendCommand(Command->Command) < 0) {
    printf("%s: error\r\n",Command->Command);
    return NULL;
  }
  token = strsep(&Params,"\\");
  if (token) {
    if (wismo218_sendParams(token) < 0) {
      printf("%s - %s: error\r\n",Command->Command,Params);
      return NULL;
    }
  }
  if (wismo218_sendParams(CR) < 0) {
    printf("%s - CR: error\r\n",Command->Command);
    return NULL;
  }
  statusCode = NUMBER_TO_CALL_SENT_STATUS;
  commandExitCode = 1;
  return &commandExitCode;
}

/**
 * \brief      CMGS response handler
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
void* response_CMGS(void* cmd, void* data, void* answer)
{
  char *token;

  if (cmd == NULL) return NULL;
  if (data) {
    //arnGsmRemoteCommand_t *Command = cmd;
    char* Dt = data;
    switch (statusCode) {
      case NUMBER_TO_CALL_SENT_STATUS:
	token = strsep(&Params,"\\");
	if (token) {
	  //printf("%s\r\n",token);
	  if (wismo218_sendParams(token) < 0) {
	    printf("token %s: error\r\n",token);
	    return NULL;
	  }
	  if (wismo218_sendParams(CTRL_Z) < 0) {
	    printf("CTRL_Z: error\r\n");
	    return NULL;
	  }
	statusCode = TEXT_BODY_SENT_STATUS;
	}
	break;
      case TEXT_BODY_SENT_STATUS:
	//printf("data: %s\r\n",data);
	statusCode = FIRST_ACK_RECEIVED_STATUS;
	break;
      case FIRST_ACK_RECEIVED_STATUS:
	//printf("data: %s\r\n",data);
	statusCode = TEXT_BODY_RECEIVED_STATUS;
	break;
      case TEXT_BODY_RECEIVED_STATUS:
	//printf("data: %s\r\n",data);
	statusCode = TOTAL_MGS_NUM_RECIVED_STATUS;
	break;
      case TOTAL_MGS_NUM_RECIVED_STATUS:
	printf("%s%s\r\n",CMGS_info_message_IT[CMGS_info_message_TOTAL_NUM], &(Dt[7]));
	statusCode = OK_ERROR_RECEIVED_STATUS;
	break;
      case OK_ERROR_RECEIVED_STATUS:
	printf("%s:%s\r\n",CMGS_info_message_IT[CMGS_info_message_SENDING_REPORT],data);
	if (answer) {
	  // action to load meaningful information into answer structure
	}
	statusCode = NO_STATUS;
	break;
      default:
	break;
    }
    commandExitCode = 1;
    return &commandExitCode;
  }
  printf("No data available.\r\n");
  return NULL;
}

/** @} */
