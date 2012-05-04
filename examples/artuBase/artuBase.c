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
 * $Id: artuBase.c,v 1.3 2010/01/14 18:18:51 nifi Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio GIovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-02-13
 * Updated : $Date: 2012/02/13 18:18:51 $
 *           $Revision: 1.0 $
 */

#include <string.h>
#include <ctype.h>

#include "contiki.h"

#include "dev/leds.h"
#include "dev/temperature-sensor.h"
#include "dev/halleffect-sensor.h"
#include "dev/voltage-sensor.h"
#include "dev/eeprom.h"

#include "CommandDef.h"
#include "CommandList.h"
#include "WismoManager.h"
#include "CommandManager.h"
#include "simple_atoi.h"

#include "eepromLayout.h"
#include "eepromManager.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif
#define COMMAND_BUFFER_LEN 32
static char cnstCmd[COMMAND_BUFFER_LEN];// = "+CMGS=\"+393358350919\"\\Alarm!!!!";
static arnGsmRemoteCommand_t* CurrentCommand;
static void temperatureSensorHadler(struct sensors_sensor* sensor);
static void hallEffectHandler(struct sensors_sensor* sensor);
static void VoltageSensorHandler(struct sensors_sensor* sensor);

static void idleHandler(void);
static void wismoEventHandler(arnGsmRemoteResponse_t* Answer);

static unsigned char PhoneNumberChecker(unsigned char* phn);
	#define PHN_ALL_FF				1
	#define PHN_NOT_STARTING_WITH_PLUS		2
	#define PHN_OK					3
	#define PHN_NOT_ALLOWED				4

static unsigned char isPhNumberAllowed(unsigned char* phn);

typedef enum aBStatus {
  aB_NONE = 0, //0
  aB_CHECK_EEPROM, //1
  aB_CHECKFORALLOWEDNUMBERS, //2
  aB_WAITFORMASTERNUMBER, //3
  aB_SEND_MGL_FOR_WMN, //4
  aB_WAITFORSMS, //5
  aB_SEND_MGL, //6
  aB_READSMS, //7
  aB_SEND_MGR, //8
  aB_ERASEALLSMS, //9
  aB_SEND_MGD, //10
  aB_SEND_MGS, //11
  aB_ERROR,
} aBStatus_t;
static void sendATCommand(const char* cmd, aBStatus_t exitsts);
static aBStatus_t aBStatus;
static enum {
  aBeC_NONE = 0,
  aBec_INCOHERENT_MASTERNUMER,
  aBec_MASTERNUMER_NOTALLOWED,
} errCode;

static struct {
  unsigned char : 4;
  unsigned char TempLow : 1;
  unsigned char TempHigh : 1;
  unsigned char PhNumberAllowed : 1;
  unsigned char hwinitDone : 1;
} flags;

#define IDLE_POLLING_TIME	1 * CLOCK_SECOND
static struct etimer idlePollingTimer;

static const unsigned short PHN_OFFSET[TOTAL_ALLOWED_NUMBERS] = {
  PHONENUMBER_M_OFFSET,
  PHONENUMBER_1_OFFSET,
  PHONENUMBER_2_OFFSET,
  PHONENUMBER_3_OFFSET
};

#define OUTA	LEDS_RED		// see platform/arnNanoM/dev/led-arch.c and the schematic
#define OUTB	LEDS_YELLOW		// see platform/arnNanoM/dev/led-arch.c and the schematic
#define OUTC	LEDS_GREEN		// see platform/arnNanoM/dev/led-arch.c and the schematic

static signed short tempThLower;
static signed short tempThHigher;
#define TEMP_SENDING_TIME	(((unsigned short)60) * CLOCK_SECOND)
static struct etimer TempSendingTimer;
static unsigned char phnIndex;
static void initTempTh(void);
static signed short TempRawToScaled(unsigned short raw);


static const char NUM_NON_AMESSO_MSG[] = "Numero non ammesso"; 

PROCESS(artuDemo_process, "artuBase");
AUTOSTART_PROCESSES(&artuDemo_process);

PROCESS_THREAD(artuDemo_process, ev, data)
{
  PROCESS_EXITHANDLER(goto exit);
  PROCESS_BEGIN();

  printf("Process Initialization...\r\n");
  WismoManager_Init();
  CommandManager_Init();
  printf("Process Initialization done.\n\r");

  CurrentCommand = NULL;
  flags.hwinitDone = 0;
  flags.PhNumberAllowed = 0;
  flags.TempHigh = 0;
  flags.TempLow = 0;

  leds_on(LEDS_RED); 
  phnIndex = 0;

  while(1) {
    PROCESS_WAIT_EVENT();

    if (ev == wismo218_status_event) {
      if (data != NULL) {
	if ((*((wismo218_status_t*)data)) == init_done) {
	  flags.hwinitDone = 1;
	  /*
	   *  The idle status is initialized to NONE at the end of HW initialization
	   */
	  aBStatus = aB_NONE;
	  /*
	   *  At this point of execution for sure NONE error code detected
	   */
	  errCode = aBeC_NONE;
	  /*
	   *  The idle polling timer is started only after all the initialization
	   */
	  etimer_set(&idlePollingTimer, IDLE_POLLING_TIME);
	}
      }
      /* 
       *  Wait until all processes have handled the event
       */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
    else if (ev == PROCESS_EVENT_TIMER) {
      if (data) {
	struct etimer* timer = data;
	if (timer == &idlePollingTimer) {
	  etimer_set(&idlePollingTimer, IDLE_POLLING_TIME);
	  idleHandler();
	}
	else if (timer == &TempSendingTimer) {
	  
	}
      }
    }
    else if(ev == sensors_event) {
      if ((flags.hwinitDone == 1) && (data != NULL)) {
	struct sensors_sensor* sensor = (struct sensors_sensor*)data;
	if (!strcmp(sensor->type,TEMPERATURE_SENSOR)) {
	  temperatureSensorHadler(sensor);
	}
	else if (!strcmp(sensor->type,HALLEFFECT_SENSOR)) {
	  hallEffectHandler(sensor);
	}
	else if (!strcmp(sensor->type,VOLTAGE_SENSOR)) {
	  VoltageSensorHandler(sensor);
	}
      }
      /* 
       *  Wait until all processes have handled the event
       */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
    else if(ev == wismo218_data_event) {
      if ((flags.hwinitDone == 1) && (data != NULL) &&  CurrentCommand) {
	arnGsmRemoteResponse_t* Answer = (arnGsmRemoteResponse_t*)data;
	wismoEventHandler(Answer);
      }
      /* 
       *  Wait until all processes have handled the event
       */
      if(PROCESS_ERR_OK == process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      }
    }
  }

 exit:
  leds_on(LEDS_ALL); // inverted logic
  PROCESS_END();
}


void idleHandler(void)
{
  //BEGIN
  //printf("aBStatus:%d\r\n",aBStatus);
  //END
  switch (aBStatus) {
    case aB_NONE:
      aBStatus = aB_CHECK_EEPROM;
    break;
    case aB_CHECK_EEPROM: {
      unsigned char tmpBff[INITKEY_SIZE];
      memset(tmpBff,0,sizeof(tmpBff));
      eeprom_read(INITKEY_OFFSET,tmpBff,sizeof(tmpBff));
      if (!strncmp((char*)tmpBff,(char*)INIT_EEPROM_CHECK_KEY,sizeof(tmpBff))) {
	printf("Eeprom verificata\n\r");
	initTempTh();
	aBStatus = aB_CHECKFORALLOWEDNUMBERS;
      }
      else {
	unsigned short tmp = ((1 << TLOEN_0_BIT) || (1 << TLOEN_0_BIT));
	aB_eraseEEPROM();
	eeprom_write(INITKEY_OFFSET,INIT_EEPROM_CHECK_KEY,INITKEY_SIZE);
	eeprom_write(INITFLAG_OFFSET,(unsigned char*)&tmp,INITFLAG_SIZE);
	tmp = 100; eeprom_write(TEMP_TH_LOWER_OFFSET,(unsigned char*)&tmp,TEMP_TH_LOWER_SIZE);
	tmp = 0; eeprom_write(TEMP_TH_HIGHER_OFFSET,(unsigned char*)&tmp,TEMP_TH_HIGHER_SIZE);
	printf("Eeprom Inizializzata\n\r");
      }
    }
    break;
    case aB_CHECKFORALLOWEDNUMBERS: {
      unsigned char resphnchk;
      unsigned char tmpBff[PHONENUMBER_SIZE];
      memset(tmpBff,0,sizeof(tmpBff));
      eeprom_read(PHONENUMBER_M_OFFSET,tmpBff,sizeof(tmpBff));
      resphnchk = PhoneNumberChecker(tmpBff);
      if (resphnchk == PHN_ALL_FF) {
	aBStatus = aB_WAITFORMASTERNUMBER;
	printf("Artu e' in attesa di Master Number...\r\n");
      }
      else if (resphnchk == PHN_NOT_STARTING_WITH_PLUS) {
	aBStatus = aB_ERROR;
	errCode = aBec_INCOHERENT_MASTERNUMER;
      }
      else if (resphnchk == PHN_NOT_ALLOWED) {
	aBStatus = aB_ERROR;
	errCode = aBec_MASTERNUMER_NOTALLOWED;
      }
      else if (resphnchk == PHN_OK) {
	aBStatus = aB_WAITFORSMS;
	printf("Master Number OK\r\n");
      }
    }
    break;
    case aB_WAITFORMASTERNUMBER: {
      sendATCommand("+CMGL=\"ALL\"",aB_SEND_MGL_FOR_WMN);
    }
    break;
    case aB_READSMS: {
      sendATCommand("+CMGR=1",aB_SEND_MGR);
    }
    break;
    case aB_WAITFORSMS: {
      //printf("SMS generico atteso\r\n");
      sendATCommand("+CMGL=\"ALL\"",aB_SEND_MGL);
    }
    break;
    case aB_ERASEALLSMS: {
      sendATCommand("+CMGD=1,4",aB_SEND_MGD);
    }
    break;
    case aB_ERROR: {
      switch (errCode) {
	case aBec_INCOHERENT_MASTERNUMER:
	  printf("Errore: numero master non ammesso\r\n");
	  errCode = aBeC_NONE;
	break;
	case aBec_MASTERNUMER_NOTALLOWED:
	  printf("Errore: numero master corrotto\r\n");
	  errCode = aBeC_NONE;
	break;
	default:
	break;
      }
    }
    break;
    case aB_SEND_MGL:
    break;
    default:
    break;
  }
}

void wismoEventHandler(arnGsmRemoteResponse_t* Answer)
{
  if (Answer->type == TYPEVAL_MGL) {
    if (aBStatus == aB_SEND_MGL_FOR_WMN) {
      if (Answer->Param1.index > 0) {printf("SMS da:%s\r\n",Answer->Param3.phonenumber);aBStatus = aB_READSMS;}
      else {aBStatus = aB_WAITFORMASTERNUMBER;}
    }
    else if (aBStatus == aB_SEND_MGL) {
      if (Answer->Param1.index > 0) {
	flags.PhNumberAllowed = isPhNumberAllowed((unsigned char*)(Answer->Param3.phonenumber));
	if (flags.PhNumberAllowed) {printf("%s ammesso\r\n",Answer->Param3.phonenumber); aBStatus = aB_READSMS;}
	else {printf("%s non ammesso\r\n",Answer->Param3.phonenumber); aBStatus = aB_ERASEALLSMS;}
      }
      else {aBStatus = aB_WAITFORSMS;}
    }
    memset(Answer,0,sizeof(arnGsmRemoteResponse_t));
    CurrentCommand = NULL;
  }
  else if (Answer->type == TYPEVAL_MGR) {
    if (aBStatus == aB_SEND_MGR) {
      unsigned char code = 0;
      char* token;
      char* data = Answer->Param3.text;
      //printf("Testo:%s\r\n",Answer->Param3.text);
      token = strsep(&data," ");
      if (token) code = simple_atoi(token);
      printf("Codice:%d\r\n",code);
      switch(code) {
	case 1:
	case 2:
	case 3:
	case 4:
	  token = Answer->Param3.text + strlen(token) + 1;
	  if (token) {
	    if (token[0] == '+') {
	      printf("dato:%s\r\n",token);
	      eeprom_write(PHN_OFFSET[code - 1],(unsigned char*)token,PHONENUMBER_SIZE);
	      if (code == 1) printf("Master Number impostato\r\n");
	      else printf("Numero %d impostato\r\n",(code - 1));
	    }
	  }
	break;
	case 10:
	  token = Answer->Param3.text + strlen(token) + 1;
	  if (token) {
	    if (!strcmp(token,"ON") ||
	      !strcmp(token,"on")
	    ) {
	      printf("2:%s\r\n",token);
	      leds_off(OUTA); // inverted logic
	    }
	    else if (!strcmp(token,"OFF") ||
	      !strcmp(token,"off")
	    ) {
	      printf("2:%s\r\n",token);
	      leds_on(OUTA); // inverted logic
	    }
	  }
	break;
	case 11:
	  token = Answer->Param3.text + strlen(token) + 1;
	  if (token) {
	    if (!strcmp(token,"ON") ||
	      !strcmp(token,"on")
	    ) {
	      printf("2:%s\r\n",token);
	      leds_off(OUTB); // inverted logic
	    }
	    else if (!strcmp(token,"OFF") ||
	      !strcmp(token,"off")
	    ) {
	      printf("2:%s\r\n",token);
	      leds_on(OUTB); // inverted logic
	    }
	  }
	break;
	case 12:
	  token = Answer->Param3.text + strlen(token) + 1;
	  if (token) {
	    if (!strcmp(token,"ON") ||
	      !strcmp(token,"on")
	    ) {
	      printf("2:%s\r\n",token);
	      leds_off(OUTC); // inverted logic
	    }
	    else if (!strcmp(token,"OFF") ||
	      !strcmp(token,"off")
	    ) {
	      printf("2:%s\r\n",token);
	      leds_on(OUTC); // inverted logic
	    }
	  }
	break;
	case 20: {
	  token = Answer->Param3.text + strlen(token) + 1;
	  if (token) {
	    tempThLower = simple_atoi(token);
	    printf("dato:%d\r\n",tempThLower);
	    eeprom_write(TEMP_TH_LOWER_OFFSET,(unsigned char*)&tempThLower,TEMP_TH_LOWER_SIZE);
	  }
	}
	break;
	case 21: {
	  token = Answer->Param3.text + strlen(token) + 1;
	  if (token) {
	    tempThHigher = simple_atoi(token);
	    printf("dato:%d\r\n",tempThHigher);
	    eeprom_write(TEMP_TH_HIGHER_OFFSET,(unsigned char*)&tempThHigher,TEMP_TH_HIGHER_SIZE);
	  }
	}
	break;
	default:
	break;
      }
      aBStatus = aB_ERASEALLSMS;
      memset(Answer,0,sizeof(arnGsmRemoteResponse_t));
      flags.PhNumberAllowed = 0;
      CurrentCommand = NULL;
    }
  }
  else if (Answer->type == TYPEVAL_MGD) {
    printf("Canc:%s\r\n",Answer->Param3.text);
    aBStatus = aB_WAITFORSMS;
    memset(Answer,0,sizeof(arnGsmRemoteResponse_t));
    CurrentCommand = NULL;
  }
  else if (Answer->type == TYPEVAL_MGS) {
    printf("report Invio:%s\r\n",Answer->Param3.text);
    aBStatus = aB_ERASEALLSMS;
    memset(Answer,0,sizeof(arnGsmRemoteResponse_t));
    CurrentCommand = NULL;
  }
  else if (Answer->type == TYPEVAL_NONE) {
    printf("Risposta sconosciuta\r\n");
  }
}

void sendATCommand(const char* cmd, aBStatus_t exitsts)
{
  char* CommandParameters;
  char* Dt = cnstCmd;
  strcpy(cnstCmd,cmd);
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
	  aBStatus = exitsts;
	}
      }
    }
  }
}

void temperatureSensorHadler(struct sensors_sensor* sensor)
{
  int v = TempRawToScaled(sensor->value(SENSORS_READY));
  //printf("Temperature value:%d\r\n",v);
  /*
   *  Higher Threshold
   */
  if (!flags.TempHigh) {
    if (v > tempThHigher) {
      //+CMGS=\"+393358350919\"\\Alarm!!!!
      char tmpbuff[48];
      strcpy(tmpbuff,"+CMGS=\"");
      eeprom_read(PHN_OFFSET[phnIndex],((unsigned char*)tmpbuff) + 7,PHONENUMBER_SIZE);
      //NOTE maybe a coherence check on the phone number can be usefull
      if (PhoneNumberChecker(((unsigned char*)tmpbuff) + 7) == PHN_OK) {
	strcpy(tmpbuff + 7 + PHONENUMBER_SIZE,"\"\\Temp H");
	flags.TempHigh = 1;
	sendATCommand(tmpbuff,aB_SEND_MGS);
	etimer_set(&TempSendingTimer, TEMP_SENDING_TIME);
	printf("T HI\r\n");
      }
      else {printf("%s\r\n",NUM_NON_AMESSO_MSG);}
    }
  }
  else {
    if (v < (tempThHigher - 2)) {
      flags.TempHigh = 0;
      phnIndex = 0;
      //sendATCommand(
    }
  }
  
  /*
   *  Lower Threshold
   */
  if (!flags.TempLow) {
    if (v > tempThLower) {
      flags.TempLow = 1;
      //sendATCommand(
    }
  }
  else {
    if (v < (tempThLower - 2)) {
      flags.TempLow = 0;
      phnIndex = 0;
      //sendATCommand(
    }
  }
}

signed short TempRawToScaled(unsigned short raw)
{
  /*
   *  Insert here the scaling formula or table
   */
  return raw;
}

unsigned char PhoneNumberChecker(unsigned char* phn)
{
  unsigned char i;
  /*
   * Check if the nnumber is all 0xff, clear value of thye eeprom
   */
  for (i = 0; i < PHONENUMBER_SIZE; i++) {
    if (phn[i] != 0xff) break;
  }
  if (i == PHONENUMBER_SIZE) return PHN_ALL_FF;
  if (phn[0] != '+') return PHN_NOT_STARTING_WITH_PLUS;
  /*
   * Check the consistency of the remaining part of the Phone Number
   */
  for (i = 1; i < PHONENUMBER_SIZE; i++) {
    if (!isdigit(phn[i])) break;
  }
  if (i == PHONENUMBER_SIZE) return PHN_OK;
  for ( ; i < PHONENUMBER_SIZE; i++) {
    if (phn[i] != ' ') break;
  }
  if (i == PHONENUMBER_SIZE) return PHN_OK;
  return PHN_NOT_ALLOWED;
  
}

unsigned char isPhNumberAllowed(unsigned char* phn)
{
  unsigned char tmpBff[PHONENUMBER_SIZE];
  unsigned char i;
  for (i = 0; i < TOTAL_ALLOWED_NUMBERS; i++) {
    memset(tmpBff,0,sizeof(tmpBff));
    eeprom_read(PHN_OFFSET[i],tmpBff,sizeof(tmpBff));
    if (PhoneNumberChecker(tmpBff) == PHN_ALL_FF) return 0;
    if (!memcmp(tmpBff,phn,PHONENUMBER_SIZE)) return 1;
  }
  return 0;
}

void initTempTh(void)
{
  eeprom_read(TEMP_TH_LOWER_OFFSET,(unsigned char*)&tempThLower,sizeof(tempThLower));
  eeprom_read(TEMP_TH_HIGHER_OFFSET,(unsigned char*)&tempThHigher,sizeof(tempThHigher));
  if (tempThLower == 0xffff) tempThLower = 0;
  else if ((tempThHigher != 0xffff) && (tempThLower >= tempThHigher)) tempThLower = 0;
}


void hallEffectHandler(struct sensors_sensor* sensor)
{
  printf("HallEffect Counter:%d\r\n",sensor->value(SENSORS_READY));
  //+CMGS=\"+393358350919\"\\Alarm!!!!
  char tmpbuff[48];
  strcpy(tmpbuff,"+CMGS=\"");
  eeprom_read(PHN_OFFSET[0],((unsigned char*)tmpbuff) + 7,PHONENUMBER_SIZE);
  //NOTE maybe a coherence check on the phone number can be usefull
  if (PhoneNumberChecker(((unsigned char*)tmpbuff) + 7) == PHN_OK) {
    strcpy(tmpbuff + 7 + PHONENUMBER_SIZE,"\"\\Allarme attivo");
    sendATCommand(tmpbuff,aB_SEND_MGS);
    etimer_set(&TempSendingTimer, TEMP_SENDING_TIME);
    printf("Allarme attivo\r\n");
  }
  else printf("%s\r\n",NUM_NON_AMESSO_MSG);
}

void VoltageSensorHandler(struct sensors_sensor* sensor)
{
  //printf("Voltage:%d\r\n",sensor->value(SENSORS_READY));
}