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
 * $Id: AntonelloApp.c,v 1.0 2012/03/14 18:18:51 fabiogiovagini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio Giovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-02-13
 * Updated : $Date: 2012/02/13 18:18:51 $
 *           $Revision: 1.0 $
 */


#include <string.h>
#include "contiki.h"
#include "dev/halleffect-sensor.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

#include "WismoManager.h"
#include "AntonelloApp.h"

static AntonelloAppStatus_t Status;
static struct etimer timer;
#define CHECKING_TIME	5 * CLOCK_SECOND

PROCESS(antonello_process, "AntonelloApp");
//AUTOSTART_PROCESSES(&antonello_process);


PROCESS_THREAD(antonello_process, ev, data)
{
  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();
  Status = antApp_Init;
  etimer_set(&timer, CHECKING_TIME);
  
  printf("AntonelloApp started.\n\r");
  while(1) {
    PROCESS_WAIT_EVENT();

    if (ev == PROCESS_EVENT_TIMER) {
      etimer_set(&timer, CHECKING_TIME);
      switch (Status) {
	case antApp_Initdone:
	  Status = antApp_Idle;
	  break;
	case antApp_Idle:
	  break;
	default: break;
      }
    }
    else if (ev == sensors_event) {
      if (data != NULL) {
	struct sensors_sensor* sensor = (struct sensors_sensor*)data;
	if (!strcmp(sensor->type,HALLEFFECT_SENSOR)) {
	  printf("Counter:%d\r\n",sensor->value(0));
	}
      }
      /* Wait until all processes have handled the sensor_event line event */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
    else if (ev == wismo218_status_event) {
      if (data != NULL) {
	 wismo218_status_t status = (*((wismo218_status_t*)data));
	 if (status == init_done) {Status = antApp_Initdone;printf("wismo reset done.\n\r");}
      }
      /* Wait until all processes have handled the sensor_event line event */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
    else if (ev == wismo218_data_event) {
      if (data != NULL) {
	printf("New data from wismo are available\r\n");
      }
      /* Wait until all processes have handled the sensor_event line event */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
  }

 exit:
  printf("Exit AntonelloApp\n\r");
  PROCESS_END();
}



void
AntonelloApp_Init(void)
{
  process_start(&antonello_process, NULL);  
}

