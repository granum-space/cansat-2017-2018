-include $(TOPDIR)/Make.defs

# blinky

ASRCS =
CSRCS =
MAINSRC = lsm_test_main.c

CONFIG_XYZ_PROGNAME ?= lsm_test$(EXEEXT)
PROGNAME = $(CONFIG_XYZ_PROGNAME)

# application info

APPNAME = lsm_test
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 4096

include $(APPDIR)/Application.mk
