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

#include "contiki.h"
#include "WismoManager.h"
#include "CommandManager.h"
#include "AntonelloApp.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

PROCESS(gsmremote_process, "gsmRemote");

AUTOSTART_PROCESSES(&gsmremote_process);

PROCESS_THREAD(gsmremote_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Process Initialization...\r\n");
  WismoManager_Init();
  CommandManager_Init();
  AntonelloApp_Init();
  printf("Process Initialization done.\n\r");

  PROCESS_END();
}
