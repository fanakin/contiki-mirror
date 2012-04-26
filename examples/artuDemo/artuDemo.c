/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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

#include <string.h>

#include "contiki.h"

#include "dev/leds.h"
#include "dev/temperature-sensor.h"
#include "dev/halleffect-sensor.h"

#include "CommandDef.h"
#include "CommandList.h"
#include "WismoManager.h"
#include "CommandManager.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif
static char cnstCmd[] = "+CMGS=\"+393358350919\"\\Alarm!!!!";
static arnGsmRemoteCommand_t* CurrentCommand;

PROCESS(artuDemo_process, "artuDemo");

AUTOSTART_PROCESSES(&artuDemo_process);

PROCESS_THREAD(artuDemo_process, ev, data)
{
  static struct {
    unsigned char : 5;
    unsigned char messageAlarmTrigger : 1;
    unsigned char hwinitDone : 1;
    unsigned char on : 1;
  } flags;

  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();

  printf("Process Initialization...\r\n");
  WismoManager_Init();
  CommandManager_Init();
  printf("Process Initialization done.\n\r");

  flags.hwinitDone = 0;
  flags.on = 0;
  flags.messageAlarmTrigger = 0;
  leds_on(LEDS_RED); 
  while(1) {
    PROCESS_WAIT_EVENT();

    if (ev == wismo218_status_event) {
      if (data != NULL) {
	if ((*((wismo218_status_t*)data)) == init_done) {
	  flags.hwinitDone = 1;
	}
      }
      /* Wait until all processes have handled the event */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
    else if(ev == sensors_event) {
      if ((flags.hwinitDone == 1) && (data != NULL)) {
	struct sensors_sensor* sensor = (struct sensors_sensor*)data;
	if (!strcmp(sensor->type,TEMPERATURE_SENSOR)) {
	  int v = sensor->value(SENSORS_READY);
	  //printf("Temperature value:%d\r\n",v);
	  if (!flags.on) {
	    if (v > 600) {
	      flags.on = 1;
	      leds_off(LEDS_RED); // inverted logic
	    }
	  }
	  else {
	    if (v < 400) {
	      flags.on = 0;
	      leds_on(LEDS_RED); // inverted logic
	      if (flags.messageAlarmTrigger == 0) {
		char* CommandParameters;
		char* Dt = cnstCmd;
		flags.messageAlarmTrigger = 1;
		CurrentCommand = arnCommand(Dt);
		//printf("%s:%s\n\r",__FUNCTION__,Dt);
		if (CurrentCommand) {
		  char* pres; 
		  CommandParameters = arnCommandParameters(CurrentCommand,Dt);
		  if (CurrentCommand->Command_handler) {
		    pres = CurrentCommand->Command_handler(CurrentCommand,CommandParameters);
		    if (pres) {
		      if (*pres == 1) {
			process_post(PROCESS_BROADCAST, wismo218_command_event, CurrentCommand);
		      }
		    }
		  }
		}
		else printf("Unimplemented Command\n\r");
		CurrentCommand = 0;
	      }
	    }
	  }
	}
	else if (!strcmp(sensor->type,HALLEFFECT_SENSOR)) {
	  printf("HallEffect Counter:%d\r\n",sensor->value(SENSORS_READY));
	}
      }

      /* Wait until all processes have handled the event */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
  }

 exit:
  leds_off(LEDS_ALL);
  PROCESS_END();
}
