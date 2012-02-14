/*
 * Copyright (c) 2002, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the Contiki OS
 *
 * $Id: contiki-main.c,v 1.14 2011/01/21 14:19:57 nvt-se Exp $
 *
 */

//#include <stdio.h>
#include <unistd.h>

#include "contiki.h"

#include "net/netstack.h"
#include "dev/serial-line.h"
#include "net/uip.h"

#include "dev/button-sensor.h"
#include "dev/watchdog.h"
#include "dev/pir-sensor.h"
#include "dev/vib-sensor.h"
#include "dev/button-sensor.h"
#include "dev/wismo218.h"
#include "../../cpu/h836064/inh8300h.h"
#include "../../cpu/h836064/dev/sci3-putchar.h"
#include "../../cpu/h836064/dev/sci3_2-putchar.h"
#include "printf.h"

void h836064_cpu_init(void);
/*---------------------------------------------------------------------------*/
static void
print_processes(struct process * const processes[])
{
  printf("Starting");
  while(*processes != NULL) {
    printf(" '%s'", (*processes)->name);
    processes++;
  }
  /* Needed to force link with putchar */
  printf("\n\r");
}
/*---------------------------------------------------------------------------*/


PROCINIT(&etimer_process);

//SENSORS(&pir_sensor, &vib_sensor, &button_sensor);

/*---------------------------------------------------------------------------*/
int
os_main(void)
{

  h836064_cpu_init();
  watchdog_periodic();
  
  init_printf(NULL,pf_putc);
 
  printf("Starting Contiki...\n\r");
 
  process_init();

  ctimer_init();
  watchdog_periodic();

  //netstack_init();
  
  procinit_init();

  sci3_set_input(serial_line_input_byte);
  serial_line_init();
  
  sci3_2_set_input(wismo128_line_input_byte);
  wismo218_init();

  watchdog_periodic();
  reset_imask_ccr();

  printf(CONTIKI_VERSION_STRING " started.\r\n");

  print_processes(autostart_processes);
  autostart_start(autostart_processes);
  watchdog_periodic();
  
  
  while(1) {
    int n;
    watchdog_periodic();

    n = process_run();

    etimer_request_poll();
  }
  
  return 0;
}
/*---------------------------------------------------------------------------*/
void
log_message(char *m1, char *m2)
{
  printf("%s%s\n", m1, m2);
}
/*---------------------------------------------------------------------------*/
void
uip_log(char *m)
{
  printf("%s\n", m);
}
/*---------------------------------------------------------------------------*/
