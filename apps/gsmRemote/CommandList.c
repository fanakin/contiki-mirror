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
  {aGRCG_generic,"HELP",COMMAND_LIST_VERSION COMMAND_GENERAL_DESCR},
  {aGRCG_wismo218,"+CSMS",""},
  {aGRCG_wismo218,"+CSTA",""},
  {aGRCG_wismo218,"+CPIN","Stato del PIN"},
  {aGRCG_wismo218,"+CSQ","Livello di segnale. Non accetta ?"},
  {aGRCG_wismo218,"+CIND",""},
  {aGRCG_wismo218,"+CGMI",""},
  {aGRCG_wismo218,"+CGMM",""},
  {aGRCG_wismo218,"+CGMR",""},
  {aGRCG_wismo218,"+CGSN",""},
  {aGRCG_wismo218,"+CSMS",""},
  {aGRCG_wismo218,"+CPMS",""},
  {aGRCG_wismo218,"+CMGF",""},
  {aGRCG_wismo218,"+CSCA",""},
  {aGRCG_wismo218,"+CSCB",""},
  {aGRCG_wismo218,"+CSMP",""},
  {aGRCG_wismo218,"+CSDH",""},
  {aGRCG_wismo218,"+CSAS",""},
  {aGRCG_wismo218,"+CRES",""},
  {aGRCG_wismo218_CTRLZ,"+CMGS","Invia messaggio diretto. Ex."},
  {aGRCG_wismo218,"+CMSS","Invia messaggio da memoria."},
  {aGRCG_wismo218_CTRLZ,"+CMGW","Scrive messaggio in memoria."},
  {aGRCG_wismo218,"+CMGD","Cancella messaggio."},
  {aGRCG_wismo218,"+CNMI",""},
  {aGRCG_wismo218,"+CMGL","Mostra lista messaggi in memoria"},
  {aGRCG_wismo218,"WSMOF",""},
  {aGRCG_wismo218,"WSMON",""},
  {aGRCG_wismo218,"WSMRST",""},
  {NULL}
};

