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


extern void printHelp(void*);

#include "CommandList.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif
#define COMMAND_LIST_VERSION "Command List 0.1\r\n"
#define COMMAND_GENERAL_DESCR "Tutti i comandi accettano l'opzione ? e =?. \
ES. +CPIN? oppure +CPIN=?\
\r\n"
const arnGsmRemoteCommand_t CommandList[] = {
/*  0*/  {aGRCG_generic,"HELP",COMMAND_LIST_VERSION COMMAND_GENERAL_DESCR,printHelp},
/*  1*/  {aGRCG_wismo218,"+CSMS","",NULL},
/*  2*/  {aGRCG_wismo218,"+CSTA","",NULL},
/*  3*/  {aGRCG_wismo218,"+CPIN","Stato del PIN",NULL},
/*  4*/  {aGRCG_wismo218,"+CSQ","Livello di segnale. Non accetta ?",NULL},
/*  5*/  {aGRCG_wismo218,"+CIND","",NULL},
/*  6*/  {aGRCG_wismo218,"+CGMI","",NULL},
/*  7*/  {aGRCG_wismo218,"+CGMM","",NULL},
/*  8*/  {aGRCG_wismo218,"+CGMR","",NULL},
/*  9*/  {aGRCG_wismo218,"+CGSN","",NULL},
/* 10*/  {aGRCG_wismo218,"+CSMS","",NULL},
/* 11*/  {aGRCG_wismo218,"+CPMS","",NULL},
/* 12*/  {aGRCG_wismo218,"+CMGF","",NULL},
/* 13*/  {aGRCG_wismo218,"+CSCA","",NULL},
/* 14*/  {aGRCG_wismo218,"+CSCB","",NULL},
/* 15*/  {aGRCG_wismo218,"+CSMP","",NULL},
/* 16*/  {aGRCG_wismo218,"+CSDH","",NULL},
/* 17*/  {aGRCG_wismo218,"+CSAS","",NULL},
/* 18*/  {aGRCG_wismo218,"+CRES","",NULL},
/* 19*/  {aGRCG_wismo218,"+FMI","",NULL},
/* 20*/  {aGRCG_wismo218,"+FMM","",NULL},
/* 21*/  {aGRCG_wismo218,"+FMR","",NULL},
/* 22*/  {aGRCG_wismo218,"+IPR","",NULL},
/* 23*/  {aGRCG_wismo218,"+PSSLEEP","Sleep mode",NULL},
/* 24*/  {aGRCG_wismo218_CTRLZ,"+CMGS","Invia messaggio diretto. Due spazi tre numero e testo. Ex. +CMGS=\"+393341234567\"  PROVA MSG",NULL},
/* 25*/  {aGRCG_wismo218,"+CMSS","Invia messaggio da memoria.",NULL},
/* 26*/  {aGRCG_wismo218_CTRLZ,"+CMGW","Scrive messaggio in memoria.",NULL},
/* 27*/  {aGRCG_wismo218,"+CMGD","Cancella messaggio.",NULL},
/* 28*/  {aGRCG_wismo218,"+CNMI","",NULL},
/* 29*/  {aGRCG_wismo218,"+CMGL","Mostra lista messaggi in memoria",NULL},
/* 30*/  {aGRCG_wismo218,"+CCLK","Agisce su RTC",NULL},
/* 31*/  {aGRCG_wismo218,"+CMGR","Legge il messaggio",NULL},
/* 32*/  {aGRCG_wismo218,"E","Echo on(1)/off(0)",NULL},
/* 33*/  {aGRCG_wismo218,"gOFF","",NULL},
/* 34*/  {aGRCG_wismo218,"gON","",NULL},
/* 35*/  {aGRCG_wismo218,"gRST","",NULL},
/* 36*/  {aGRCG_wismo218,"gERST","",NULL},
/* 37*/  {aGRCG_wismo218,"gEWM","Abilita sms di benevenuto all'accensione",NULL},
/* 38*/  {aGRCG_wismo218,"gDWM","Disbilita sms di benevenuto all'accensione",NULL},
/* 39*/  {aGRCG_wismo218,"gSPHN","Imposta in Eeprom il numero da messaggiare",NULL},
/* 30*/  {aGRCG_wismo218,"gGPHN","Mostra il numero da messaggiare",NULL},
/* 41*/  {aGRCG_wismo218,"gEIDL","Abilita IDLE",NULL},
/* 42*/  {aGRCG_wismo218,"gDIDL","Disbilita IDLE",NULL},
/* 43*/  {aGRCG_wismo218,"TST0","",NULL},
/*   */  {aGRCG_none,NULL,NULL,NULL}
};

