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
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"

#include <stdio.h> /* For printf() */
#include "net/netstack.h"
#include "sicslowpan.h"
#include "net/routing/routing.h"


#include "sys/ctimer.h"
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);


static struct ctimer downward_timer;
//static bool radio_isOn = false;
static void downward_timer_callback()
{
	printf("rev data timeout!\n");
    ctimer_reset(&downward_timer);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
//  static struct etimer timer;

  PROCESS_BEGIN();
	printf("start rev timeout timer\n");

	  printf("ctimer up\n");
//  etimer_set(&timer, 1 * CLOCK_SECOND);
	  ctimer_set(&downward_timer, 2 * CLOCK_SECOND, downward_timer_callback, NULL);
  while(1)
  {
//	  if(radio_isOn == false){
//		  printf("radio on\r\n");
//		  radio_isOn = true;
//	  }


	  PROCESS_YIELD();
	    /* Wait for the periodic timer to expire and then restart the timer. */
	    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
	    //etimer_reset(&timer);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
