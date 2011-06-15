TARGET		:=	libbricks.so
ATARGET		:=	libbricks.a
BUILD		:=	build
DATA		:=	data
SOURCES		:=	source source/io source/threading
INCLUDES	:=	include

DEFINES		:= -g -O3 -Wall -fPIC -D_GNU_SOURCE #-std=gnu++0x
LIBS		:= 
LDFLAGS		:= -shared

MAKENAME	:= Makefile.pc.mk

ifeq ($(BUILD),$(notdir $(CURDIR)))
include ../common.mk
else
include ./common.mk
endif
