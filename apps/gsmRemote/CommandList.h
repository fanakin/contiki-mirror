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
typedef enum arnGsmRemoteCommandGruop {
	aGRCG_none = 0, /*!< none group */
	aGRCG_generic, /*!< generic group; help, ... */
	aGRCG_eeprom, /*!< eeprom group; eeprom management commands */
	aGRCG_wismo218, /*!< wism218 management and AT COMMANDS group */
	aGRCG_wismo218_CTRLZ, /*!< wism218 management and AT COMMANDS group but with an extention to insert message text*/
	aGRCG_unknown /*!< unknown group */
} arnGsmRemoteCommandGruop_t;

typedef struct arnGsmRemoteCommand {
	arnGsmRemoteCommandGruop_t Gruop;
	const char *CommandString;
	const char *HelpString;
} arnGsmRemoteCommand_t;

#define MAX_CMD_PARAMS_ALLOWED_LEN	128
extern const arnGsmRemoteCommand_t CommandList[];
#endif