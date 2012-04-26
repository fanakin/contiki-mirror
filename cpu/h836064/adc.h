/*
 * Copyright (c) 2012, Aurion .s.r.l (Bologna - Italy)
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
 * @(#)$Id: adc.h,v 0.1 2012/02/03 10:28:32 fabiogiovagnini Exp $
 */
/** \file /adc.h
 * \brief Header for A/D converter.
 *
 * <!--(C) COPYRIGHT 2012 Aurion s.r.l.. All rights reserved.        -->
 */
/** \addtogroup adc
 * Sample A/D converter driver.
 *
 * See adc.h for source code.
 *
 * \note it is the implementation for h836064
 *
 *
 * To use the ADC system, include this file and ensure that
 * initADC is called whenever the microcontroller is
 * started before the subsysterm (timer handler) is started.  
 *
 *
 *@{
 */


#ifndef __ADC_H__
#define __ADC_H__

typedef void (*adcChannelHandler_t) (int value);

/** \brief Initialize the A/D Converter hardware register to start conversion
 * 
 * Hardware inizialization for continues 10 bit conversion
 * 
 */
void initADC (void);

/** \brief Sampling function
 * 
 * Call it in a clock handelr to have a clocked sampling
 * 
 */
void ADC_Inth(void);

/** \brief Sampling handler set fuction
 * 
 * for each channel we set the sampling handler being impelemnted 
 * in the sensor implementation file
 * 
 * @param index  the number of the channel between 0 and 7 (8 channel present on the microcontroller)
 * 
 * @return func the real function for sample the data of the channel and sennting the real sampling rate
 */
void setADCChannelHandle(unsigned char index, adcChannelHandler_t func);

#endif

/** @} // END addtogroup
 */
