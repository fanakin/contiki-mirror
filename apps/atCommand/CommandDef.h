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
 * $Id: CommandDef.h,v 0.1 2012/04/06 16:18:51 fabiogiovagnini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio Giovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-04-06
 * Updated : $Date: 2012/04/06 16:18:51 $
 *           $Revision: 1.0 $
 */

#include "contiki.h"

#ifndef __COMMANDDEF_H__
#define __COMMANDDEF_H__


/*! \struct arnGsmRemoteCommand
*  \brief Structure for generic command description
*
* This structure collect all the allowed command of the system \n
* with the corresponding handlers \n
* See const arnGsmRemoteCommand_t CommandList[].
*
*/
typedef struct arnGsmRemoteCommand {
	const char *Command; /*!< string defining the command */
	const char *HelpString; /*!< string giving help to the user */
	void* (*Command_handler) (void *,void *); /*!< handler for command; if NULL no handler is allowed; first parameter is arnGsmRemoteCommand_t* to the command; second is char* to the command parameter line; return value: NULL if no event needs to be broadcast; a char* with non zero pointed value if an event needs to be broadcasted */
	void* (*Response_handler) (void *,void *,void *); /*!< handler for command; if NULL no handler is allowed; first parameter is arnGsmRemoteCommand_t* to the command; second is char* to data and the third is the command response */
} arnGsmRemoteCommand_t;

/*! \struct arnGsmRemoteResponse
*  \brief Structure for generic command response
*
* This structure collect the meaningful part of the \n
* response of a command if required \n
* It is the 3rd argument of the response void* (*Response_handler) (void *,void *,void *) \n
*
*/
typedef struct arnGsmRemoteResponse {
	#define PARAM1_LEN	16
	union {
	  char array[PARAM1_LEN];
	  char phonenumber[PARAM1_LEN];
	} Param1;
} arnGsmRemoteResponse_t;

#define MAX_CMD_PARAMS_ALLOWED_LEN	128
extern const char LF[];
extern const char CR[];
extern const char CTRL_Z[];

/**
 * Exit code for each impelemnted command if the need to dispatch an event
 *
 * When the developer implement a pair void* command_XXX(void* cmd, void* data)
 * and void* response_XXX(void* cmd, void* data) if the handler needs tp dispatch an event
 * the handler needs to set commandExitCode to 1 and return &commandExitCode.
 */
extern unsigned char commandExitCode;

/**
 * Status for the command and response evolution for commands with 
 * composite and complex behaviour.
 *
 * When the developer implement a pair void* command_XXX(void* cmd, void* data)
 * and void* response_XXX(void* cmd, void* data) if the handler needs to manage
 * a complex response (example, the +CMSG command) the command handler sets the initial
 * status value, and the response manages the several different states each of which important form
 * different aspect.
 * 
 */
extern unsigned char statusCode;
#endif