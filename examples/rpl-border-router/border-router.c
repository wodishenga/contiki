/*
 * Copyright (c) 201, RISE SICS
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

#include "contiki.h"

/* Log configuration */
#include "sys/log.h"
#include "rpl-dag-root.h"


#include "contiki.h"
#include "contiki-net.h"

#include "net/routing/rpl-lite/rpl.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-sr.h"

/* Log configuration */
#include "sys/log.h"
#include "stdlib.h"

#include <string.h>

#include "lib/list.h"


#define LOG_MODULE "RPL BR"
#define LOG_LEVEL LOG_LEVEL_INFO

#define ENABLE_UDP 1
#if ENABLE_UDP
#include "simple-udp.h"
#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678
static struct simple_udp_connection udp_conn;
#endif


/*---------------------------------------------------------------------------*/
/*节点发送数据过来，server回复的回掉函数*/
#if ENABLE_UDP
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  unsigned count = *(unsigned *)data;
  LOG_INFO("Received request %u from ", count);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
#if WITH_SERVER_REPLY
  LOG_INFO("Sending response %u to ", count);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  simple_udp_sendto(&udp_conn, &count, sizeof(count), sender_addr);
#endif /* WITH_SERVER_REPLY */
}
#endif
/*---------------------------------------------------------------------------*/

/* Declare and auto-start this file's process */
PROCESS(contiki_ng_br, "Contiki-NG Border Router");
AUTOSTART_PROCESSES(&contiki_ng_br);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(contiki_ng_br, ev, data)
{
 // static struct etimer timer;
//  static long unsigned int second;
  PROCESS_BEGIN();
  //list_init(nodelist_backup);
#if BORDER_ROUTER_CONF_WEBSERVER
  PROCESS_NAME(webserver_nogui_process);
  process_start(&webserver_nogui_process, NULL);
#endif /* BORDER_ROUTER_CONF_WEBSERVER */
  LOG_INFO("Contiki-NG Border Router started\n");

/*如果使能了UDP，则需要注册server的回掉函数*/
#if ENABLE_UDP
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);
#endif
  
  /*每60秒打印一次路由列表*/
/*  etimer_set(&timer, CLOCK_SECOND * 60);
  while(1) {
	  second++;
	  printf("root links(%lu second):\r\n",second);
	  if(rpl_dag_root_is_root()) {
	    if(uip_sr_num_nodes() > 0) {
		  uip_sr_node_t *link;
	      LOG_INFO("links: %u routing links in total (%s)\n", uip_sr_num_nodes(),"adamsxiaomi");
	      while(link != NULL) {
	        char buf[1000];
	        uip_sr_link_snprint(buf, sizeof(buf), link);
	        LOG_INFO("links: %s\n", buf);
	        link = uip_sr_node_next(link);
	      }
	      LOG_INFO("links: end of list\n");
	    } else {
	      LOG_INFO("No routing links\n");
	    }
	  }
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }
*/
  PROCESS_END();
}

