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
 * $Id: utilfunctions.c,v 1.0 2012/03/01 18:18:51 fabiogiovagnini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio Giovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-03-01
 * Updated : $Date: 2012/03/01 18:18:51 $
 *           $Revision: 1.0 $
 */

#include <string.h>

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

int simple_atoi(char* s)
{
  #define MAX_LEN_ALLOWED 11
  // to convert input like '1234' to 1234.
  int ret = 0;
  int len = 0;
  signed char signum = 1;
  if (s == NULL) return -1;
  while (1) {
    if (!(*s)) break;
    if (len == MAX_LEN_ALLOWED) break;
    if ((*s) == '-') signum = -1;
    else if ((*s >= '0')&& (*s <= '9')) ret = (ret*10) + (*s - '0');
    s++;
    len++;
  }
  return (ret * signum);
}