CONTIKI_PROJECT = nullnet-unicast nullnet-broadcast nullnet-unicast-coordinator
all: $(CONTIKI_PROJECT)

CONTIKI = ../..

PLATFORMS_EXCLUDE = nrf52dk
#MODULES += os/services/shell
#use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
#MAKE_MAC ?= MAKE_MAC_CSMA
MAKE_MAC = MAKE_MAC_TSCH
MAKE_NET = MAKE_NET_NULLNET

include $(CONTIKI)/Makefile.include
