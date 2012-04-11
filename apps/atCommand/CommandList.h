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
 * $Id: CommandManager.h,v 0.1 2012/02/13 18:18:51 fabiogiovagnini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio GIovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-02-13
 * Updated : $Date: 2012/02/13 18:18:51 $
 *           $Revision: 1.0 $
 */

#include "contiki.h"

#ifndef __COMMANDLIST_H__
#define __COMMANDLIST_H__

/*! \fn arnGsmRemoteCommand_t* arnCommand(const char* CMD)
 *  \brief Search a command string CMD amomng the allowed system commands
 *
 * @param CMD string starting with a command to look for
 * @return Pointer to arnGsmRemoteCommand_t struct if CMD is found, NULL otherwise.
 */
arnGsmRemoteCommand_t* arnCommand(const char* CMD);


/*! \fn const char* arnCommandParameters(const arnGsmRemoteCommand_t* foundCommand, char* CMD)
 *  \brief Extract the string containing the parameter of the command
 *
 * @param foundCommand pointer to the structure of the found command
 * @param CMD string starting with a command and continuing with the parameters
 * @return Pointer to the parameters of the command if any, NULL otherwise.
 * 
 * Ex: 
 * ...
 * char lookedForCommand[] = "+CPIN?";
 * char* CommandParameters;
 * arnGsmRemoteCommand_t* foundCommand;
 * foundCommand = arnCommand(lookedForParams);
 * if (foundCommand) {
 *    CommandParameters = arnCommandParameters(foundCommand,lookedForParams);
 *    if (CommandParameters) // Command Found and The Command has Parameters
 *    else // Command Found and The Command has NO Parameters
 * }
 * else // Command NOT Found
 */
char* arnCommandParameters(const arnGsmRemoteCommand_t* foundCommand, char* CMD);

#endif