-include $(TOPDIR)/Make.defs

# blinky

ASRCS =
CSRCS =
MAINSRC = mpu_test_main.c

CONFIG_XYZ_PROGNAME ?= mpu_test$(EXEEXT)
PROGNAME = $(CONFIG_XYZ_PROGNAME)

# application info

APPNAME = mpu_test
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 4096

include $(APPDIR)/Application.mk
