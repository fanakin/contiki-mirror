/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 * This file is part of the Contiki operating system.
 *
 * @(#)$Id: contiki-conf.h,v 1.12 2009/09/09 21:11:24 adamdunkels Exp $
 */

#ifndef __CONTIKI_CONF_H__
#define __CONTIKI_CONF_H__

typedef unsigned char   uint8_t;
typedef signed char     sint8_t;
typedef signed char     int8_t;
typedef unsigned short  uint16_t;
typedef signed short    sint16_t;
typedef signed short    int16_t;
typedef unsigned long   uint32_t;
typedef signed long     sint32_t;
typedef signed long     int32_t;

typedef uint8_t  u8_t;
typedef sint8_t  s8_t;
typedef uint16_t u16_t;
typedef sint16_t s16_t;
typedef uint32_t u32_t;
typedef sint32_t s32_t;
typedef u16_t    uip_stats_t;
typedef unsigned long    clock_time_t;

#define CLOCK_CONF_SECOND 1000

#define WITH_UIP 0

#define CCIF
#define CLIF

/* Not part of C99 but actually present */
//int strcasecmp(const char*, const char*);

#endif /* __CONTIKI_CONF_H__ */
