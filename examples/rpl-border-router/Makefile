CONTIKI_PROJECT = border-router
all: $(CONTIKI_PROJECT)
CONTIKI = ../..

# The BR is either native or embedded, and in the latter case must support SLIP
PLATFORMS_EXCLUDE = nrf52dk
#MAKE_MAC=MAKE_MAC_TSCH
# Include RPL BR moduleK
MODULES += os/services/rpl-border-router
# Include webserver module
MODULES_REL += webserver
MODULES += os/services/shell
# Include optional target-specific module
include $(CONTIKI)/Makefile.identify-target
MODULES_REL += $(TARGET)

include $(CONTIKI)/Makefile.include
