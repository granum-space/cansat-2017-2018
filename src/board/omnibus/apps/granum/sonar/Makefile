-include $(TOPDIR)/Make.defs

# blinky

ASRCS =
CSRCS =
MAINSRC = sonar_main.c

CONFIG_XYZ_PROGNAME ?= sonar$(EXEEXT)
PROGNAME = $(CONFIG_XYZ_PROGNAME)

# application info

APPNAME = sonar
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 2048

include $(APPDIR)/Application.mk
