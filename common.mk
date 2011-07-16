ifneq ($(strip $(LIBS)),)
LIBS_CFLAGS		:= $(shell pkg-config --cflags $(LIBS))
LIBS_LDFLAGS	:= $(shell pkg-config --libs $(LIBS))
endif
CFLAGS		+= $(LIBS_CFLAGS) $(DEFINES) $(foreach dir,$(LIBRARIES),-I../lib/$(dir)/include)
CXXFLAGS	+= $(LIBS_CFLAGS) $(DEFINES) $(foreach dir,$(LIBRARIES),-I../lib/$(dir)/include)
LDFLAGS		+= $(LIBS_LDFLAGS) $(foreach dir,$(LIBRARIES),-L../lib/$(dir)) $(foreach lib,$(LIBRARIES),-l$(lib))
ifneq ($(USECLANG),)
CXX			:= clang++
CC			:= clang
else
CXX			:= g++
CC			:= gcc
endif
LD			:= $(CXX)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export VPATH	:=	$(foreach dir,$(SOURCES),../$(dir)) \
					$(foreach dir,$(DATA),../$(dir))
export DEPSDIR	:=	../$(BUILD)

HPPFILES	:=	$(foreach dir,$(PCHSOURCES),$(notdir $(wildcard $(dir)/*.hpp)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

export OFILES	:=	$(addsuffix .o,$(BINFILES))	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o)
export GCHFILES	:=	$(addsuffix .gch,$(HPPFILES))

ifneq ($(USECLANG),)
export PCHFLAGS	:=	$(foreach pch,$(GCHFILES) $(addprefix ../,$(foreach dir,$(PCHINCLUDES),$(wildcard $(dir)/*.hpp.gch))),-include-pch $(pch))
endif

INCLUDES		+=	$(PCHSOURCES)
export INCLUDE	:=	$(foreach dir,$(INCLUDES) $(PCHINCLUDES), -I../$(dir)) -I../$(BUILD)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
ifneq ($(ATARGET),)
export AOUTPUT	:=	$(CURDIR)/$(ATARGET)
endif

.PHONY: $(BUILD) clean

ifeq ($(MAKENAME),)
MAKENAME		:= Makefile
endif

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/$(MAKENAME) -j8

clean:
	@echo "[RM]  $(notdir $(OUTPUT))"
	@rm -fr $(BUILD) $(OUTPUT) $(AOUTPUT)

run: $(BUILD)
	@$(OUTPUT)

debug: $(BUILD)
	@gdb -ex run $(OUTPUT)

else

DEPENDS	:=	$(OFILES:.o=.d) $(GCHFILES:.gch=.d)

$(OUTPUT): $(GCHFILES) $(OFILES) $(AOUTPUT)
	@echo "[LD]  $(notdir $@)"
	@$(LD) $(LDFLAGS) $(OFILES) -o $(OUTPUT)

$(OFILES) : $(GCHFILES)

ifneq ($(AOUTPUT),)
$(AOUTPUT): $(OFILES)
	@echo "[AR]  $(notdir $@)"
	@$(AR) -rs $@ $(OFILES)
endif

%.o : %.cpp
	@echo "[CXX] $(notdir $<)"
	@$(CXX) -c -MMD -MP -MF $(DEPSDIR)/$*.d $(INCLUDE) $(CXXFLAGS) $(PCHFLAGS) -o $@ $<

%.o : %.c
	@echo "[CC]  $(notdir $<)"
	@$(CC) -c -MMD -MP -MF $(DEPSDIR)/$*.d $(INCLUDE) $(CFLAGS) $(PCHFLAGS) -o $@ $<

%.hpp.gch : %.hpp
	@echo "[CXX] $(notdir $<)"
	@$(CXX) -c -MMD -MP -MF $(DEPSDIR)/$*.hpp.d $(INCLUDE) $(CFLAGS) -o $@ $<

%.jpg.o	:	%.jpg
	@echo "[B2O] $(notdir $<)"
	@$(bin2o)

-include $(DEPENDS)

endif
