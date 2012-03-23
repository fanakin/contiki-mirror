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
#include "dev/eeprom.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

#include "CommandList.h"
#include "WismoManager.h"
#include "CommandManager.h"
#include "eepromLayout.h"
#include "AntonelloApp.h"

static AntonelloAppStatus_t Status;
static struct etimer timer;
#define CHECKING_TIME	1 * CLOCK_SECOND
static wismo218Cmd_t Wismo218Command = {"","","",0x03};

const char QUESTION_MARK[] = {0x3f,0x0d,0x00};
const char EQUAL_QUESTION_MARK[] =  {0x3d,0x3f,0x0d,0x00};
extern const char CTRL_Z[];

static int parseWismo218Answer(wismo218Ans_t* Ans);
#define ANS_OK			0
#define ANS_NOT			-1
#define ANS_INVALID		-2

static char ParamBuffer[20];
static char subParam1Buffer[48];
static antAppRTC_t DataTime;
static int getDataTime(const char* str,antAppRTC_t* Data);

struct antIdleActivite {
  unsigned long SchedTimerForData;
  unsigned int counter;
  struct {
    unsigned short : 13;
    unsigned short EnIdle: 1;
    unsigned short SendMsg: 1;
    unsigned short WelMsgSent: 1;
  } FLAG;
} antIdleStructure;

#define ONE_MINUTE			60 // seconds
#define ONE_HOUR			(60 * ONE_MINUTE)
#define HALF_A_DAY			(12 * ONE_HOUR)
#define A_DAY				(24 * ONE_HOUR)

#define TIME_FOR_DATA_SCHEDULE		(4 * ONE_HOUR)

PROCESS(antonello_process, "AntonelloApp");
//AUTOSTART_PROCESSES(&antonello_process);


PROCESS_THREAD(antonello_process, ev, data)
{
  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();
  Status = antApp_Init;
  etimer_set(&timer, CHECKING_TIME);
  memset (&antIdleStructure,0,sizeof(antIdleStructure));
  
  printf("AntonelloApp started.\n\r");
  while(1) {
    PROCESS_WAIT_EVENT();

    if (ev == PROCESS_EVENT_TIMER) {
      etimer_set(&timer, CHECKING_TIME);
      switch (Status) {
	case antApp_Initdone:
	  Status = antApp_CheckEepromInit;
	  break;
	case antApp_CheckEepromInit: {
	  unsigned char bff[INITKEY_SIZE];
	  eeprom_read(INITKEY_OFFSET,bff,sizeof(bff));
	  if (strncmp((const char*)bff,"BABE",sizeof(bff))) Status = antApp_EepromInit;
	  else Status = antApp_PowerOn;
	  }
	  break;
	case antApp_EepromInit: {
	  unsigned char bff[INITKEY_SIZE] = {'B','A','B','E'};
	  eeprom_write(INITKEY_OFFSET,bff,sizeof(bff));
	  printf("Initialization Tag written.\n\r");
	  Status = antApp_CheckEepromInit;
	  }
	  break;
	case antApp_PowerOn: {
	  Wismo218Command.Cmd = (char*)(CommandList[3].CommandString); //+CPIN
	  Wismo218Command.Params = (char*)QUESTION_MARK; //"?"
	  Wismo218Command.subParams1 = 0;
	  Wismo218Command.ActivationFlags |= 0x03;
	  Status = antApp_PostCommad;
	  }
	  break;
	case antApp_PostCommad: {
	  /* Broadcast event */
	  process_post(PROCESS_BROADCAST, wismo218_command_event, &Wismo218Command);
	  Status = antApp_WaitAnswer;
	  }
	  break;
	case antApp_WaitAnswer: break;
	case antApp_Idle:
	  //printf("Idle\r\n");
	  if (!antIdleStructure.FLAG.EnIdle) break;
	  if (antIdleStructure.SchedTimerForData++ == TIME_FOR_DATA_SCHEDULE) {
	    antIdleStructure.SchedTimerForData = 0;
	    antIdleStructure.FLAG.SendMsg = 1;
	    Status = antApp_Getdatatime;
	  }
	  break;
	case antApp_SendInitWM: {
	  printf("Send WM\r\n");
	  memset(ParamBuffer,0,sizeof(ParamBuffer));
	  //memset(subParam1Buffer,0,sizeof(subParam1Buffer)); see +CCLK command
	  ParamBuffer[0] = '=';ParamBuffer[1] = '"';
	  eeprom_read(PHONENUMBER_OFFSET,((unsigned char*)ParamBuffer) + 2 ,PHONENUMBER_SIZE);
	  ParamBuffer[2 + PHONENUMBER_SIZE] = '"';
	  ParamBuffer[3 + PHONENUMBER_SIZE] = 13;
	  strcat(subParam1Buffer," W218 Powered on.");
	  Wismo218Command.Cmd = (char*)(CommandList[19].CommandString); //+CMGS
	  Wismo218Command.Params =ParamBuffer;
	  Wismo218Command.subParams1 = subParam1Buffer;
	  strcat(Wismo218Command.subParams1,CTRL_Z);
	  Wismo218Command.ActivationFlags |= 0x07;
	  //printf("%s\r\n",Wismo218Command.Params);
	  //printf("%s\r\n",Wismo218Command.subParams1);
	  Status = antApp_PostCommad;
	  }
	  break;
	case antApp_SendMsg: {
	  printf("Send Msg\r\n");
	  memset(ParamBuffer,0,sizeof(ParamBuffer));
	  //memset(subParam1Buffer,0,sizeof(subParam1Buffer)); see +CCLK command
	  ParamBuffer[0] = '=';ParamBuffer[1] = '"';
	  eeprom_read(PHONENUMBER_OFFSET,((unsigned char*)ParamBuffer) + 2 ,PHONENUMBER_SIZE);
	  ParamBuffer[2 + PHONENUMBER_SIZE] = '"';
	  ParamBuffer[3 + PHONENUMBER_SIZE] = 13;
	  strcat(subParam1Buffer," Cnt:");
	  sprintf(subParam1Buffer + strlen(subParam1Buffer),"%d",antIdleStructure.counter);
	  Wismo218Command.Cmd = (char*)(CommandList[19].CommandString); //+CMGS
	  Wismo218Command.Params =ParamBuffer;
	  Wismo218Command.subParams1 = subParam1Buffer;
	  strcat(Wismo218Command.subParams1,CTRL_Z);
	  Wismo218Command.ActivationFlags |= 0x07;
	  antIdleStructure.FLAG.SendMsg = 0;
	  //printf("%s\r\n",Wismo218Command.Params);
	  //printf("%s\r\n",Wismo218Command.subParams1);
	  Status = antApp_PostCommad;
	  }
	  break;
	case antApp_Getdatatime: {
	  printf("Get datatime\r\n");
	  memset(ParamBuffer,0,sizeof(ParamBuffer));
	  ParamBuffer[0] = '?';
	  ParamBuffer[1] = 13;
	  Wismo218Command.Cmd = (char*)(CommandList[25].CommandString); //+CCLK
	  Wismo218Command.Params = ParamBuffer;
	  Wismo218Command.subParams1 = 0;
	  Wismo218Command.ActivationFlags |= 0x03;
	  //printf("%s\r\n",Wismo218Command.Params);
	  //printf("%s\r\n",Wismo218Command.subParams1);
	  Status = antApp_PostCommad;
	  }
	  break;
	case antApp_Getsignallevel : {
	  printf("Get signal quality\r\n");
	  memset(ParamBuffer,0,sizeof(ParamBuffer));
	  ParamBuffer[0] = 13;
	  Wismo218Command.Cmd = (char*)(CommandList[4].CommandString); //+CSQ
	  Wismo218Command.Params = ParamBuffer;
	  Wismo218Command.subParams1 = 0;
	  Wismo218Command.ActivationFlags |= 0x03;
	  //printf("%s\r\n",Wismo218Command.Params);
	  //printf("%s\r\n",Wismo218Command.subParams1);
	  Status = antApp_PostCommad;
	  }
	  break;
	case antApp_Error:
	  printf("Error\r\n");
	  break;
	default: break;
      }
    }
    else if (ev == sensors_event) {
      if (data != NULL) {
	struct sensors_sensor* sensor = (struct sensors_sensor*)data;
	if (!strcmp(sensor->type,HALLEFFECT_SENSOR)) {
	  antIdleStructure.counter = sensor->value(0);
	  printf("Counter:%d\r\n",sensor->value(0));
	}
      }
    }
    else if (ev == wismo218_status_event) {
      if (data != NULL) {
	 wismo218_status_t status = (*((wismo218_status_t*)data));
	 if (status == init_done) {Status = antApp_Initdone;printf("wismo reset done.\n\r");}
      }
    }
    else if (ev == wismo218_data_event) {
      if (data != NULL) {
	wismo218Ans_t *Ans = (wismo218Ans_t*)data;
	if (ANS_OK == parseWismo218Answer(Ans)) {printf("ANS OK\r\n");}
      }
      /* Wait until all processes have handled the wismo218_data_event line event */
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


int
parseWismo218Answer(wismo218Ans_t* Ans)
{
  //BEGIN DEBUG
  //printf("\r\n%s:%s\r\n",__FUNCTION__,Ans->Answer);
  //END DEBUG
  if (strncmp((const char*)Ans->Answer,Wismo218Command.Cmd,strlen(Wismo218Command.Cmd))) {
    //if (Ans->Answer) printf("%s\r\n",Ans->Answer);
    return ANS_INVALID;
  }
  else {
    if (!strncmp((const char*)Wismo218Command.Cmd,(char*)(CommandList[3].CommandString),5)) { //+CPIN
      if (Ans->Answer) {
	Ans->Answer += strlen(Wismo218Command.Cmd);
	Ans->Answer += 2;
	printf("%s\r\n",Ans->Answer);
      }
      Status = antApp_Getsignallevel;
      if (!strncmp((const char*)Ans->Answer,"READY",5)) return ANS_OK;
      Status = antApp_Error;
    }
    else if (!strncmp((const char*)Wismo218Command.Cmd,(char*)(CommandList[4].CommandString),5)) { //+CSQ
      if (Ans->Answer) {
	Ans->Answer += strlen(Wismo218Command.Cmd);
	Ans->Answer += 2;
	printf("%s\r\n",Ans->Answer);
      }
      Status = antApp_Getdatatime;
      return ANS_OK;
      ///TODO
      // If teh signal less than a threshold maybe can be usefull to freeze the applicazion
      //if (!strncmp((const char*)Ans->Answer,"READY",5)) return ANS_OK;
      //Status = antApp_Error;
    }
    else if (!strncmp((const char*)Wismo218Command.Cmd,(char*)(CommandList[25].CommandString),5)) { // +CCLK
      union {
	unsigned short us;
	unsigned char bff[sizeof(unsigned short)];
      } tmpbff;
      if (Ans->Answer) {
	Ans->Answer += strlen(Wismo218Command.Cmd);
	Ans->Answer += 2;
	//printf("%s\r\n",Ans->Answer);
	memset(subParam1Buffer,0,sizeof(subParam1Buffer));
	strncpy(subParam1Buffer,Ans->Answer + 1,17);
	if (getDataTime(Ans->Answer,&DataTime)) {
	  printf("%d/%d/%d-%d:%d:%d\r\n",DataTime.Year,DataTime.Month,DataTime.Day,DataTime.Hour,DataTime.Min,DataTime.Sec);
	}
      }
      if (antIdleStructure.FLAG.WelMsgSent == 0) {
	eeprom_read(INITFLAG_OFFSET,tmpbff.bff,sizeof(tmpbff));
	if (tmpbff.us & (1 << ANTAPP_IDEL_EN_BIT)) {antIdleStructure.FLAG.EnIdle = 1;}
	else  {antIdleStructure.FLAG.EnIdle = 0;}
	if (tmpbff.us & (1 << WM_EN_BIT)) {etimer_set(&timer, 2 * CHECKING_TIME);  Status = antApp_SendInitWM;}
	else {antIdleStructure.FLAG.WelMsgSent = 1; Status = antApp_Idle;}
      }
      else if (antIdleStructure.FLAG.SendMsg == 1) {
	Status = antApp_SendMsg;
      }
      else Status = antApp_Idle;
    }
    else if (!strncmp((const char*)Wismo218Command.Cmd,(char*)(CommandList[19].CommandString),5)) { //+CMGS
      Status = antApp_Idle;
      if (Ans->Answer) printf("%s\r\n",Ans->Answer);
      if (!strncmp((const char*)(Ans->Answer),(char*)(CommandList[19].CommandString),5)) {antIdleStructure.FLAG.WelMsgSent = 1; return ANS_OK;}
      //Status = antApp_Error;
    }
    else {if (Ans->Answer) printf("%s\r\n",Ans->Answer);}
  }

  return ANS_NOT;
}


int getDataTime(const char* str,antAppRTC_t* Data)
{
  // str is: "yy/mm/dd,hh:mm:ss+GMT GMT in quarter of hour
  if (!str || !Data) return 0;
  str++;
  Data->Year = ((10 * (*str - 48)) + ((*(str + 1) - 48))); str += 3;
  Data->Month = ((10 * (*str - 48)) + ((*(str + 1) - 48))); str += 3;
  Data->Day = ((10 * (*str - 48)) + ((*(str + 1) - 48))); str += 3;
  Data->Hour = ((10 * (*str - 48)) + ((*(str + 1) - 48))); str += 3;
  Data->Min = ((10 * (*str - 48)) + ((*(str + 1) - 48))); str += 3;
  Data->Sec = ((10 * (*str - 48)) + ((*(str + 1) - 48))); str += 3;
  Data->GMT = ((10 * (*str - 48)) + ((*(str + 1) - 48))); str += 3;
  return 1;
}
