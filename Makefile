TARGET = DigitalClock
OBJS = src/utils.o src/error.o src/battery.o src/callbacks.o lib/glib2d/glib2d.o src/tex.o src/music.o src/main.o

LIBS = -lpng -lz -lpspgu -lm -lpspvram -lpsprtc -lpspctrl -lpsppower -lpspaudio -lpspmp3 -lpsppower

WARNING_FLAGS = -Werror -Wall -Wextra -Wno-sign-compare -Wno-format-truncation

CFLAGS = -O2 -G0 -fno-pic $(WARNING_FLAGS)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP

PSP_EBOOT_TITLE = Digital Clock
PSP_EBOOT_ICON = assets/xmb/ICON0.PNG
PSP_EBOOT_PIC1 = assets/xmb/PIC1.PNG
PSP_EBOOT_SFO = assets/xmb/CPARAM.SFO

# TODO: Add vvv
# PSP_EBOOT_ICON1 = assets/xmb/ICON1.PMF

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak