-include $(TOPDIR)/Make.defs

# blinky

ASRCS =
CSRCS = madgwick/MadgwickAHRS.c
MAINSRC = mavlink_test_main.c

CONFIG_XYZ_PROGNAME ?= mavlink_test$(EXEEXT)
PROGNAME = $(CONFIG_XYZ_PROGNAME)

# application info

APPNAME = mavlink_test
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 20480

include $(APPDIR)/Application.mk
