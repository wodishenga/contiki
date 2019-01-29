CONTIKI_PROJECT = lpm-mode

PLATFORMS_ONLY = cc26x0-cc13x0

all: $(CONTIKI_PROJECT)

#MODULES += lpm_downward
MODULES += os/net/app-layer/coap


PROJECT_SOURCEFILES += lpm_downward.c

#MODULES += os/services/simple-energest
CONTIKI = ../../../..
include $(CONTIKI)/Makefile.include
