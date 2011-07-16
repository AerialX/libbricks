TARGET		:=	test
BUILD		:=	build
DATA		:=	data
SOURCES		:=	source
INCLUDES	:=	$(SOURCES) ../include
PCHINCLUDES	:=	../build ../source

DEFINES		:= -g -O3 -Wall -fPIC # -std=c++0x
LIBS		:= 
LDFLAGS		:= -L../.. -lbricks

export LD_LIBRARY_PATH	:= ../

ifeq ($(BUILD),$(notdir $(CURDIR)))
include ../../common.mk
else
include ../common.mk
endif
