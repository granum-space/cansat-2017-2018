-include $(TOPDIR)/Make.defs

# blinky

ASRCS =
CSRCS =
MAINSRC = lumen_main.c

CONFIG_XYZ_PROGNAME ?= lumen$(EXEEXT)
PROGNAME = $(CONFIG_XYZ_PROGNAME)

# application info

APPNAME = lumen
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 2048

include $(APPDIR)/Application.mk
