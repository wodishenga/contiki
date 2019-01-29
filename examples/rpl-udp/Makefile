CONTIKI_PROJECT = udp-client udp-server
all: $(CONTIKI_PROJECT)
#MAKE_MAC=MAKE_MAC_TSCH
MODULES += os/services/shell
#MODULES += os/services/orchestra
MODULES += example/rpl-udp/udp-client.c
CONTIKI=../..
include $(CONTIKI)/Makefile.include
