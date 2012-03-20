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
 * $Id: eepromLayout.h,v 0.1 2012/02/13 18:18:51 fabiogiovagnini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio GIovagnini, Aurion s.r.l. (Bologna - Italy)
 * Created : 2012-02-13
 * Updated : $Date: 2012/02/13 18:18:51 $
 *           $Revision: 1.0 $
 */

#include "contiki.h"

#ifndef __EEPROMLAYOUT_H__
#define __EEPROMLAYOUT_H__

#define EEPROM_SIZE		128

#define INITKEY_OFFSET		0
#define INITKEY_SIZE		4

#define INITFLAG_OFFSET		(INITKEY_OFFSET + INITKEY_SIZE)
#define INITFLAG_SIZE		2
	#define WM_EN_BIT		0
	#define UNDEF_1_BIT		1
	#define UNDEF_2_BIT		2
	#define UNDEF_3_BIT		3
	#define UNDEF_4_BIT		4
	#define UNDEF_5_BIT		5
	#define UNDEF_6_BIT		6
	#define UNDEF_7_BIT		7
	#define UNDEF_8_BIT		8
	#define UNDEF_9_BIT		9
	#define UNDEF_10_BIT		10
	#define UNDEF_11_BIT		11
	#define UNDEF_12_BIT		12
	#define UNDEF_13_BIT		13
	#define UNDEF_14_BIT		14
	#define UNDEF_15_BIT		15

#define PHONENUMBER_OFFSET	(INITFLAG_OFFSET + INITFLAG_SIZE)
#define PHONENUMBER_SIZE	13


#endif