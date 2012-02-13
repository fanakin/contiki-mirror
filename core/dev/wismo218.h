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
/*! \union wismo218Cmd
*  \brief Structure defining a wismo218 command
*
* This structure defines the command the application needs to send the wismo218
*
*/
#define WISM218_CMD_LEN       6
#define WISM218_CMD_PAR_LEN   8
typedef union wismo218Cmd {
  struct _fieldCmd {
    char StartTag; /*!< Can be: +, #, * */
    char Cmd[WISM218_CMD_LEN]; /*!< It is the string of the command */
    char EndTag; /*!< Can be: =, ? */
    char OptTag; /*!< Can be: ? if EndTag is ? * */
    char Params[WISM218_CMD_PAR_LEN]; /*!< Optional parameters */
  } field;
  char Command[sizeof(struct _fieldCmd)];
} wismo218Cmd_t;

/*! \union wismo218Ans
*  \brief Structure defining a wismo218 command response
*
* This structure defines the aswer structure given by wismo218 to a command sent by the application
*
*/
#define WISM218_ANS_LEN   32
typedef union wismo218Ans {
  struct _fieldAns {
    char StartTag; /*!< Can be: +, #, * */
    char Cmd[WISM218_CMD_LEN]; /*!< It is the string of the command */
    char Params[WISM218_ANS_LEN]; /*!< Body of the answer */
  } field;
  char Answer[sizeof(struct _fieldAns)];
} wismo218Ans_t;

void wismo218_init(void);

int wismo218_sendCommand(const wismo218Cmd_t* Cmd);
#endif /* __LEDS_H__ */
