/*
 * lpm_downward.c
 *
 *  Created on: 2019Äê1ÔÂ10ÈÕ
 *      Author: adams
 */


//#include "lpm_downward.h"
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/stimer.h"
#include "sys/process.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/button-hal.h"
#include "batmon-sensor.h"
#include "board-peripherals.h"
#include "net/netstack.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/routing/routing.h"
#include "coap-engine.h"
#include "coap.h"
#include "net/ipv6/uip.h"
#include "ti-lib.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "sys/ctimer.h"

#include "net/ipv6/simple-udp.h"
#include <driverlib/cpu.h>

#include "sys/log.h"
#define LOG_MODULE "App-udp client"
#define LOG_LEVEL LOG_LEVEL_INFO
#define ENABLE_UDP 1

#if ENABLE_UDP
#define WITH_SERVER_REPLY  0
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

static struct simple_udp_connection udp_conn;

#define SEND_INTERVAL		  (20 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;
#endif
static struct ctimer downward_timer;


//#define RPL_CONF_DEFAULT_LEAF_ONLY 1
/*---------------------------------------------------------------------------*/
/* for downward data*/
#if RPL_CONF_DEFAULT_LEAF_ONLY
#define DOWNWARD_DATA_REQUEST_INTERVAL (60 * CLOCK_SECOND)
#define DOWNWARD_DATA_REQUEST_WINDOWS_SIZE  (CLOCK_SECOND / 2) //ms
static uint8_t is_waiting = 0;
static uint32_t downward_starttime = 0;
#endif
/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#if RPL_CONF_DEFAULT_LEAF_ONLY!=1
#error "lpm downward only for leaf node"
#endif


/*---------------------------------------------------------------------------*/
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
  NETSTACK_MAC.off();
  LOG_INFO("Received response %u from ", count);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  PRINTF("stop timer\n");
  ctimer_stop(&downward_timer);
  is_waiting = 0;
}
#endif
/*---------------------------------------------------------------------------*/

uint8_t is_waiting_downward()
{
	return is_waiting;
}
uint32_t get_downward_starttime()
{
	return downward_starttime;
}
static void downward_timer_callback()
{
	PRINTF("rev data timeout!\n");
    is_waiting = 0;
    ctimer_expired(&downward_timer);
    //ctimer_reset(&downward_timer);
}
void start_downward_timer(clock_time_t timeout)
{
	PRINTF("start rev timeout timer\n");
	ctimer_set(&downward_timer, timeout, downward_timer_callback, NULL);
}
void send_downward_request(clock_time_t timeout)
{
	downward_starttime = downward_starttime;
	PRINTF("is_waiting_downward\n");
    start_downward_timer(timeout);
}
PROCESS(downward_request_process, "downward request process");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(downward_request_process, ev, data)
{
  static struct etimer timer;
  PROCESS_BEGIN();
#if ENABLE_UDP
  static unsigned count;
  PRINTF("udp enable\n");
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);
#endif
  PRINTF("downward request process\n");
  etimer_set(&timer,DOWNWARD_DATA_REQUEST_INTERVAL);
  while(1)
  {
	    static uip_ipaddr_t  dest_ipaddr;
	    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr))
	    {
	    	dest_ipaddr = dest_ipaddr;
	    	PRINTF("get root addr success!\n");
#if ENABLE_UDP
	    	count++;
	        is_waiting = 1;
	        NETSTACK_MAC.on();
	        simple_udp_sendto(&udp_conn, &count, sizeof(count), &dest_ipaddr);
	        LOG_INFO("Sending request %u to ", count);
	        LOG_INFO_6ADDR(&dest_ipaddr);
	        LOG_INFO_("\n");
#endif
	    	send_downward_request(DOWNWARD_DATA_REQUEST_WINDOWS_SIZE);
	    }else{
	        PRINTF("it is not rechable\n");
	    }
	    /* Wait for the periodic timer to expire and then restart the timer. */
	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
	    etimer_reset(&timer);
  }
  PROCESS_END();
}
