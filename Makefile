#
# Makefile
#

CIRCLEHOME = ../..

OBJS	= main.o kernel.o mymulticore.o

LIBS	= $(CIRCLEHOME)/lib/libcircle.a

include ../Rules.mk

-include $(DEPS)
