-include $(TOPDIR)/Make.defs

# blinky

ASRCS =
CSRCS =
MAINSRC = gps_test_main.c

CONFIG_XYZ_PROGNAME ?= gps_test$(EXEEXT)
PROGNAME = $(CONFIG_XYZ_PROGNAME)

# application info

APPNAME = gps_test
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 4096

include $(APPDIR)/Application.mk
