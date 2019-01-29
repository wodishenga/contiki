/*
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
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "packetbuf.h"

#include "sys/log.h"
#define LOG_MODULE "App-udp server"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  0
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678
static struct simple_udp_connection udp_conn;

typedef struct SENSOR
{
	unsigned char shortAaar;
	uip_ipaddr_t  uipAddr;
	int rssi;
	unsigned long receive_count;
}sensor_t;
static unsigned char sensor_count=0;
static sensor_t sensor_info[50];
unsigned char isNodeInList(const uip_ipaddr_t *uipAddr)
{
	unsigned char i=0;
	while(i < 50)
	{
		if(uip_ip6addr_cmp(uipAddr,&sensor_info[i].uipAddr))
		{
			//LOG_INFO("node %d already in the list\r\n",sensor_info[i].shortAaar);
	        return sensor_info[i].shortAaar;
		}
		else
		{
		    i++;
		}
	}
	LOG_INFO("node is not in the list\r\n");
	return 0xff;
}
unsigned char addNodeToList(const uip_ipaddr_t *uipAddr)
{
	if(isNodeInList(uipAddr) == 0xff)
	{
		uip_ip6addr_copy(&sensor_info[sensor_count].uipAddr,uipAddr);
		sensor_info[sensor_count].shortAaar = sensor_count;
		sensor_count++;
		LOG_INFO("add %d sensor to the list\r\n",sensor_info[sensor_count].shortAaar);
		return 1;
	}
	else
	{
		//LOG_INFO("node already in the list\r\n");
		return 0;
	}
}
unsigned char addRevCount(const uip_ipaddr_t *uipAddr)
{
	unsigned char shortAddr = 0;
	shortAddr = isNodeInList(uipAddr);
    if(shortAddr<50)
    {
    	sensor_info[shortAddr].receive_count++;
    	LOG_INFO("rev %ld packet from node %d \r\n",sensor_info[shortAddr].receive_count,shortAddr);
    	return 1;
    }
	else
	{
		return 0;
	}

}
PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/
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
  unsigned char shortAddr = 0;
  int last_rssi;
  shortAddr = isNodeInList(sender_addr);
  LOG_INFO("Received request %u from ", count);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  last_rssi = (signed short)packetbuf_attr(PACKETBUF_ATTR_RSSI);
  LOG_INFO("node %d last_rssi=%d dbm  number of node = %d    ",shortAddr,last_rssi,sensor_count);
  LOG_INFO_LLADDR(packetbuf_addr(PACKETBUF_ADDR_SENDER));
  LOG_INFO_("\r\n");
  if(shortAddr > 50)//node is not in the list
  {
	  addNodeToList(sender_addr);
  }
  else
  {
	  addRevCount(sender_addr);
  }
#if WITH_SERVER_REPLY
  LOG_INFO("Sending response %u to ", count);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
  simple_udp_sendto(&udp_conn, &count, sizeof(count), sender_addr);
#endif /* WITH_SERVER_REPLY */
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();
  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
