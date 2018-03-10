-include $(TOPDIR)/Make.defs

# blinky

ASRCS =
CSRCS =
MAINSRC = blinky_main.c

CONFIG_XYZ_PROGNAME ?= blinky$(EXEEXT)
PROGNAME = $(CONFIG_XYZ_PROGNAME)

# application info

APPNAME = blinky
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 2048

include $(APPDIR)/Application.mk
