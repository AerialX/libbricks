TARGET		:=	libbricks.so
ATARGET		:=	libbricks.a
BUILD		:=	build
DATA		:=	data
SOURCES		:=	source source/io source/threading source/audio
INCLUDES	:=	include
PCHSOURCES	:=	source

DEFINES		:= -g -O3 -Wall -fPIC #-std=gnu++0x
LIBS		:= 
LDFLAGS		:= -shared

MAKENAME	:= Makefile.pc.mk

ifeq ($(BUILD),$(notdir $(CURDIR)))
include ../common.mk
else
include ./common.mk
endif
