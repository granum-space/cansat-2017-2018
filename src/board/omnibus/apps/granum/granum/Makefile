-include $(TOPDIR)/Make.defs

# blinky

ASRCS =
CSRCS = madgwick/MadgwickAHRS.c threads/madgwick_thread.c threads/sensors_thread.c threads/interfaces_thread.c
MAINSRC = granum_main.c

CONFIG_XYZ_PROGNAME ?= granum$(EXEEXT)
PROGNAME = $(CONFIG_XYZ_PROGNAME)

# application info

APPNAME = granum
PRIORITY = SCHED_PRIORITY_DEFAULT
STACKSIZE = 20480

include $(APPDIR)/Application.mk
