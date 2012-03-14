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
 * This file is part of the Contiki operating system.
 *
 * @(#)$Id: wismo218.h,v 0.1 2012/02/13 12:41:16 fabiogiovagnini Exp $
 */
/**
 * \addtogroup dev
 * @{
 */

/**
 * \defgroup Wismo218 Sierra Wireless Wismo218 Serial Interface
 *
 * This interface defines how to communicate to Wismo218 GPRS Wireless modem
 * 
 *
 * @{
 */

#ifndef __WISMO218_H__
#define __WISMO218_H__

#include "contiki.h"

/*! \union wismo218Cmd
*  \brief Structure defining a wismo218 command
*
* This structure defines the command the application needs to send the wismo218
*
*/
typedef struct wismo218Cmd {
  char* Cmd; /*!< It is the null-terminated string of the command */
  char* Params; /*!< Optional parameters; null terminated string containing ?,= and eventully other data */
  char* subParams1; /*!< Optional parameters; null terminated string containing data for SMS text  */
  unsigned char ActivationFlags; /*!< 1: field can be sent; 0: field is skipped. BIT_0:Cmd; BIT_1:Params; BIT_2:ActivationFlags */
} wismo218Cmd_t;

/*! \union wismo218Ans
*  \brief Structure defining a wismo218 command response
*
* This structure defines the aswer structure given by wismo218 to a command sent by the application
*
*/
typedef union wismo218Ans {
  struct _fieldAns {
    char StartTag; /*!< Can be: +, #, * */
  } field;
  char Answer[sizeof(struct _fieldAns)];
} wismo218Ans_t;


/**
 * Event posted when an message is received by wismo128.
 *
 * when an answer is send by wismo128 an event is posted.
 * A pointer to an aswer buffer is sent togheter with the event.
 */
extern process_event_t wismo128_event_message;
extern process_event_t wismo128_gt_message;

/**
 * Get one byte of input from the serial driver connected to wismo128.
 *
 * This function is to be called from the actual RS232 driver to get
 * one byte of serial data input.
 *
 * For systems using low-power CPU modes, the return value of the
 * function can be used to determine if the CPU should be woken up or
 * not. If the function returns non-zero, the CPU should be powered
 * up. If the function returns zero, the CPU can continue to be
 * powered down.
 *
 * \param c The data that is received.
 *
 * \return Non-zero if the CPU should be powered up, zero otherwise.
 */

int wismo128_line_input_byte(unsigned char c);

void wismo218_init(void);

int wismo218_sendCommand(const char* Cmd);

int wismo218_sendParams(const char* Params);


PROCESS_NAME(wismo128_ans_process);

#endif /* __LEDS_H__ */
