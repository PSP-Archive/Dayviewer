TARGET          = dayviewer_user
OBJS            = main.o psppaf.o exports.o

CFLAGS          = -Os -G0 -Wall -fshort-wchar
ASFLAGS         = $(CFLAGS)

BUILD_PRX       = 1
PRX_EXPORTS     = exports.exp

LIBS            = -lpspsystemctrl_user -lpsprtc -lpsppower

PSPSDK          = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak