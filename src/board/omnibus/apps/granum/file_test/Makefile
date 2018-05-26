-include $(TOPDIR)/Make.defs

# blinky

ASRCS =
CSRCS =
MAINSRC = file_test_main.c

CONFIG_XYZ_PROGNAME ?= file_test$(EXEEXT)
PROGNAME = $(CONFIG_XYZ_PROGNAME)

# application info

APPNAME = file_test
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 2048

include $(APPDIR)/Application.mk
