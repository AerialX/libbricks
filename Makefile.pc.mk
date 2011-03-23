TARGET		:=	libbricks.so
BUILD		:=	build
DATA		:=	data
SOURCES		:=	source source/io
INCLUDES	:=	include

DEFINES		:= -g -O3 -Wall -fPIC -std=c++0x
LIBS		:= 
LDFLAGS		:= -shared

MAKENAME	:= Makefile.pc.mk

-include ../common.mk
-include ../../common.mk
