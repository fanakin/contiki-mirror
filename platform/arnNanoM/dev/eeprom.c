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
 * $Id: eeprom.c,v 0.1 2012/02/14 21:11:24 fabiogiovagnini Exp $
 */

/**
 * \file
 *         eeprom implementation for arnNanoM
 * \author
 *         Fabio Giovagnini <fabio.giovagnini@aurion-tech.com>
 */

#include "36064s.h"
#include "defines-arch.h"
#include "dev/eeprom.h"
#include "dev/watchdog.h"

#define MEM_START_ADDRESS	0x00000000
#define MEM_SIZE		0x00000080


#define notWC		IO.PDR8.BIT.B6

#define IS_BUSY		IIC2.ICCR2.BIT.BBSY

// I set BBSY = 0;
// I setto SCP = 0; 
// I do a mov op
#define _STOP_()	do { \
			unsigned char tmp; \
			tmp = IIC2.ICCR2.BYTE; \
			tmp &= ~BIT7; \
			tmp &= ~BIT6; \
			IIC2.ICCR2.BYTE = tmp; \
	} while (0)

// I set BBSY = 1;
// I set SCP = 0; 
// I do a mov op
#define _START_()	do { \
			unsigned char tmp; \
			tmp = IIC2.ICCR2.BYTE; \
			tmp |= BIT7; \
			tmp &= ~BIT6; \
			IIC2.ICCR2.BYTE = tmp; \
	} while (0)

#define WRITE_PAGE_0		0xA0
#define WRITE_PAGE_1		0xA2
#define WRITE_PAGE_2		0xA4
#define WRITE_PAGE_3		0xA6
#define WRITE_PAGE_4		0xA8
#define WRITE_PAGE_5		0xAA
#define WRITE_PAGE_6		0xAC
#define WRITE_PAGE_7		0xAE

#define READ_PAGE_0			0xA1
#define READ_PAGE_1			0xA3
#define READ_PAGE_2			0xA5
#define READ_PAGE_3			0xA7
#define READ_PAGE_4			0xA9
#define READ_PAGE_5			0xAB
#define READ_PAGE_6			0xAD
#define READ_PAGE_7			0xAF

typedef enum iicbus_status {
	iics_IDLE,
	iics_DEVSEL_W,
	iics_DEVSEL_R,
	iics_ADDRESS,
	iics_DATA,
	iics_NONE,
	iics_ERROR
	}   __attribute__((packed,aligned(1))) iicbus_status_t;
typedef enum nvm_command {
	nvmc_NONE,
	nvmc_WRITE_BYTE,
	nvmc_READ_BYTE,
	nvmc_WRITE_ARRAY,
	nvmc_READ_ARRAY,
	}  __attribute__((packed,aligned(1))) nvm_command_t;
typedef enum nvm_status {
	nvms_NONE,
	nvms_READINPROGRESS,
	nvms_READDONE,
	nvms_WRITEINPROGRESS,
	nvms_WRITEDONE,
	nvms_ERROR
	}  __attribute__((packed,aligned(1))) nvm_status_t;

volatile static struct {
	nvm_command_t nvm_cmd;
	nvm_status_t nvm_sts;
	iicbus_status_t iicStatus;
	unsigned char data;
	union {
		eeprom_addr_t us;
		unsigned char uc[sizeof(eeprom_addr_t)];
		} Address;
	unsigned char DevSel_W;
	unsigned char DevSel_R;
	void* pAddress;
	unsigned short count;
	} __attribute__((packed,aligned(1))) nvm_state_str = {
		.nvm_cmd = nvmc_NONE,
		.nvm_sts = nvms_NONE,
		.iicStatus = iics_NONE,
		};


void eeprom_init(void)
{
	// di default abilito lettura e scrittura come se notWC fosse non connesso
	notWC = 0;
	// setto le porte P57 e P56 come NMOS open drai per fare SCl e SDA
	IO.PMR5.BYTE |= (BIT7 | BIT6);
	// Abilito I2C Bus
	IIC2.ICCR1.BIT.ICE = 1;
	// I2C bus MSB first
	IIC2.ICMR.BIT.MLS = 0;
	// 
	IIC2.ICMR.BIT.WAIT = 0;
	// imposto un trasfer rate di : 0x04 - 200 khz; 0x03 - 250 khz; 0x02 - 333 khz; 0x01 - 400 khz @ 16 mhz di clock freq
	IIC2.ICCR1.BIT.CKS = 0x4;
	
	nvm_state_str.nvm_cmd = nvmc_NONE;
	nvm_state_str.nvm_sts = nvms_NONE;
	nvm_state_str.iicStatus = iics_IDLE;
}

void eeprom_write(eeprom_addr_t addr, unsigned char *buf, int size)
{
while (1) {
	watchdog_periodic();
	if (nvm_state_str.nvm_sts == nvms_WRITEDONE) {
		nvm_state_str.nvm_cmd = nvmc_NONE; 
		nvm_state_str.nvm_sts = nvms_NONE; 
		return;
		}
	if (IS_BUSY || (nvm_state_str.nvm_sts != nvms_NONE)) continue;
	nvm_state_str.pAddress = buf;
	nvm_state_str.count = size;
	nvm_state_str.Address.us = addr;
	switch(nvm_state_str.Address.uc[0]) {
		case 0: nvm_state_str.DevSel_W = WRITE_PAGE_0; break;
		case 1: nvm_state_str.DevSel_W = WRITE_PAGE_1; break;
		case 2: nvm_state_str.DevSel_W = WRITE_PAGE_2; break;
		case 3: nvm_state_str.DevSel_W = WRITE_PAGE_3; break;
		case 4: nvm_state_str.DevSel_W = WRITE_PAGE_4; break;
		case 5: nvm_state_str.DevSel_W = WRITE_PAGE_5; break;
		case 6: nvm_state_str.DevSel_W = WRITE_PAGE_6; break;
		case 7: nvm_state_str.DevSel_W = WRITE_PAGE_7; break;
		default: nvm_state_str.DevSel_W = WRITE_PAGE_0; break;
		}
	// set Master transfer mode
	IIC2.ICCR1.BIT.MST = 1;
	IIC2.ICCR1.BIT.TRS = 1;
	//
	_START_();
	//
	IIC2.ICSR.BIT.TEND = 0;
	IIC2.ICIER.BIT.TEIE = 1;
	nvm_state_str.iicStatus = iics_DEVSEL_W;
	nvm_state_str.nvm_cmd = nvmc_WRITE_ARRAY;
	nvm_state_str.nvm_sts = nvms_WRITEINPROGRESS;
	IIC2.ICDRT = nvm_state_str.DevSel_W;
	}
}

void eeprom_read(eeprom_addr_t addr, unsigned char *buf, int size)
{
while (1) {
	watchdog_periodic();
	if (nvm_state_str.nvm_sts == nvms_READDONE) {
		nvm_state_str.nvm_cmd = nvmc_NONE; 
		nvm_state_str.nvm_sts = nvms_NONE; 
		return;
		}
	if (IS_BUSY || (nvm_state_str.nvm_sts != nvms_NONE)) continue;
	nvm_state_str.pAddress = buf;
	nvm_state_str.count = size;
	nvm_state_str.Address.us = addr;
	switch(nvm_state_str.Address.uc[0]) {
		case 0: nvm_state_str.DevSel_W = WRITE_PAGE_0; nvm_state_str.DevSel_R = READ_PAGE_0; break;
		case 1: nvm_state_str.DevSel_W = WRITE_PAGE_1; nvm_state_str.DevSel_R = READ_PAGE_1; break;
		case 2: nvm_state_str.DevSel_W = WRITE_PAGE_2; nvm_state_str.DevSel_R = READ_PAGE_2; break;
		case 3: nvm_state_str.DevSel_W = WRITE_PAGE_3; nvm_state_str.DevSel_R = READ_PAGE_3; break;
		case 4: nvm_state_str.DevSel_W = WRITE_PAGE_4; nvm_state_str.DevSel_R = READ_PAGE_4; break;
		case 5: nvm_state_str.DevSel_W = WRITE_PAGE_5; nvm_state_str.DevSel_R = READ_PAGE_5; break;
		case 6: nvm_state_str.DevSel_W = WRITE_PAGE_6; nvm_state_str.DevSel_R = READ_PAGE_6; break;
		case 7: nvm_state_str.DevSel_W = WRITE_PAGE_7; nvm_state_str.DevSel_R = READ_PAGE_7; break;
		default: nvm_state_str.DevSel_W = WRITE_PAGE_0; nvm_state_str.DevSel_R = READ_PAGE_0; break;
		}
	// set Master transfer mode
	IIC2.ICCR1.BIT.MST = 1;
	IIC2.ICCR1.BIT.TRS = 1;
	//
	_START_();
	//
	IIC2.ICSR.BIT.TEND = 0;
	IIC2.ICIER.BIT.TEIE = 1;
	nvm_state_str.iicStatus = iics_DEVSEL_W;
	nvm_state_str.nvm_cmd = nvmc_READ_ARRAY;
	nvm_state_str.nvm_sts = nvms_READINPROGRESS;
	IIC2.ICDRT = nvm_state_str.DevSel_W;
	}
}

void _IIC2_(unsigned long par)
{
	unsigned char tmp;
	if (IIC2.ICIER.BIT.TIE && IIC2.ICSR.BIT.TDRE) {
		IIC2.ICSR.BIT.TDRE = 0;
// test test test
//		TSTtdre++;
//END
		}
	if (IIC2.ICIER.BIT.TEIE && IIC2.ICSR.BIT.TEND) {
		IIC2.ICSR.BIT.TEND = 0;
// test test test
//		TSTtend++;
//END
		if (IIC2.ICIER.BIT.ACKBR) {
			IIC2.ICSR.BIT.STOP = 0;
			IIC2.ICIER.BIT.STIE = 1;
			_STOP_();
//BEGIN test test test
//TSTcmd = nvm_state_str.nvm_cmd;
//TSTsts = nvm_state_str.nvm_sts;
//TSTicc = nvm_state_str.iicStatus;
//END
			nvm_state_str.nvm_sts = nvms_NONE;
			nvm_state_str.nvm_cmd = nvmc_NONE;
			return;
			}
		switch (nvm_state_str.nvm_sts) {
			case nvms_READINPROGRESS:
				switch (nvm_state_str.iicStatus) {
					case iics_DEVSEL_W:
						IIC2.ICDRT = nvm_state_str.Address.uc[1];
						nvm_state_str.iicStatus = iics_ADDRESS;
						break;
					case iics_ADDRESS:
						_START_();
						IIC2.ICDRT = nvm_state_str.DevSel_R;
						nvm_state_str.iicStatus = iics_DEVSEL_R;
						break;
					case iics_DEVSEL_R:
						IIC2.ICSR.BIT.RDRF = 0;
						IIC2.ICIER.BIT.RIE = 1;
						IIC2.ICCR1.BIT.TRS = 0;
						IIC2.ICSR.BIT.TDRE = 0;
						if (nvm_state_str.nvm_cmd == nvmc_READ_BYTE) {
							IIC2.ICIER.BIT.ACKBT = 1;
							IIC2.ICCR1.BIT.RCVD = 1;
							}
						else if (nvm_state_str.nvm_cmd == nvmc_READ_ARRAY) {
							IIC2.ICIER.BIT.ACKBT = 0;
							IIC2.ICCR1.BIT.RCVD = 0;
							}
						tmp = IIC2.ICDRR; // dummy read, only to run the clock
						tmp++; // only to avoid optimization
						nvm_state_str.count--;
						nvm_state_str.iicStatus = iics_DATA;
						break;
					default: break;
					}
				break;
			case nvms_WRITEINPROGRESS:
				switch (nvm_state_str.iicStatus) {
					case iics_DEVSEL_W:
						IIC2.ICDRT = nvm_state_str.Address.uc[1];
						nvm_state_str.iicStatus = iics_ADDRESS;
						break;
					case iics_ADDRESS:
						if (nvm_state_str.nvm_cmd == nvmc_WRITE_BYTE) {
							IIC2.ICDRT = nvm_state_str.data;
							}
						else if (nvm_state_str.nvm_cmd == nvmc_WRITE_ARRAY) {
							IIC2.ICDRT = (*((unsigned char*)(nvm_state_str.pAddress)));
							nvm_state_str.pAddress++;
							}
						nvm_state_str.count--;
						nvm_state_str.iicStatus = iics_DATA;
						break;
					case iics_DATA:
						if (nvm_state_str.nvm_cmd == nvmc_WRITE_BYTE || (!(nvm_state_str.count))) {
							IIC2.ICSR.BIT.STOP = 0;
							IIC2.ICIER.BIT.STIE = 1;
							_STOP_();
							}
						else if (nvm_state_str.nvm_cmd == nvmc_WRITE_ARRAY) {
							IIC2.ICDRT = (*((unsigned char*)(nvm_state_str.pAddress)));
							nvm_state_str.pAddress++;
							nvm_state_str.count--;
							}
						nvm_state_str.iicStatus = iics_DATA;
						break;
					default: break;
					}
				break;
			default: break;
			}
		}
	if (IIC2.ICIER.BIT.RIE && IIC2.ICSR.BIT.RDRF) {
		IIC2.ICSR.BIT.RDRF = 0;
// test test test
//		TSTrdrf++;
//END
		switch (nvm_state_str.nvm_sts) {
			case nvms_READINPROGRESS:
				switch (nvm_state_str.iicStatus) {
					case iics_DATA:
						if (nvm_state_str.nvm_cmd == nvmc_READ_BYTE || (!(nvm_state_str.count))) {
							IIC2.ICIER.BIT.RIE = 0;
							IIC2.ICSR.BIT.STOP = 0;
							IIC2.ICIER.BIT.STIE = 1;
							_STOP_();
							}
						else {
							if (nvm_state_str.count == 1) {
								IIC2.ICIER.BIT.ACKBT = 1;
								IIC2.ICCR1.BIT.RCVD = 1;
								}
							(*((unsigned char*)(nvm_state_str.pAddress))) = IIC2.ICDRR;
							nvm_state_str.pAddress++;
							nvm_state_str.count--;
							}
						break;
					default: break;
					}
				break;
			default: break;
			}
		}
	if (IIC2.ICIER.BIT.STIE && IIC2.ICSR.BIT.STOP) {
		IIC2.ICIER.BIT.STIE = 0;
// test test test
//		TSTstop++;
//END
		switch (nvm_state_str.nvm_sts) {
			case nvms_READINPROGRESS:
				switch (nvm_state_str.iicStatus) {
					case iics_DATA:
						(*((unsigned char*)(nvm_state_str.pAddress))) = IIC2.ICDRR;
						IIC2.ICCR1.BIT.RCVD = 0;
						IIC2.ICCR1.BIT.MST = 0;
						IIC2.ICCR1.BIT.TRS = 0;
						nvm_state_str.nvm_sts = nvms_READDONE;
						break;
					default: break;
					}
				break;
			case nvms_WRITEINPROGRESS:
				switch (nvm_state_str.iicStatus) {
					case iics_DATA:
						IIC2.ICCR1.BIT.RCVD = 0;
						IIC2.ICCR1.BIT.MST = 0;
						IIC2.ICCR1.BIT.TRS = 0;
						nvm_state_str.nvm_sts = nvms_WRITEDONE;
						break;
					default: break;
					}
				break;
			default: break;
			}
		}
}
