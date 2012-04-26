/*
 * Copyright (c) 2012, Aurion s.r.l. - B0logna (Italy)
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
 * This file is part of the Configurable Sensor Network Application
 * Architecture for sensor nodes running the Contiki operating system.
 *
 * $Id: teperature-sensors.c,v 1.0 2012/04/26 20:15:55 fabiogiovagnini Exp $
 *
 * -----------------------------------------------------------------
 *
 * Author  : Fabio Giovagnini
 * Created : 2012-04-26
 * Updated : $Date: 2012/04/26 20:15:55 $
 *           $Revision: 1.0 $
 */

#include "adc.h"
#include "dev/temperature-sensor.h"

#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#include "../../../cpu/h836064/adc.h"
#endif

static unsigned short CurrentValue;

#define SAMPLING_TIME		250 // unit ms
static unsigned short samplingCounter;

static void temperature_sensor_handler(int value)
{
  if (--samplingCounter) return;
  samplingCounter = SAMPLING_TIME;
  if (CurrentValue != value) {
    CurrentValue = value;
    sensors_changed(&temperature_sensor);
    //printf("%d\r\n",CurrentValue);
  }
}


/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  return CurrentValue;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  switch (type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int value)
{
  switch (type) {
  case SENSORS_ACTIVE:
    if (value) {
      if(!status(SENSORS_ACTIVE)) {
      }
    }
    else {
    }
    return 1;
  case SENSORS_HW_INIT:
    setADCChannelHandle(0,temperature_sensor_handler);
    CurrentValue = 0;
    samplingCounter = SAMPLING_TIME;
    printf(TEMPERATURE_SENSOR" Activated.\r\n");
    return 1;
  default: return 0;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(temperature_sensor, TEMPERATURE_SENSOR,
	       value, configure, status);
