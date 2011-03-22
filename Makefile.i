# Application settings
TARGET		:=	$(notdir $(CURDIR)).dylib
BUILD		:=	build
DATA		:=	data
SOURCES		:=	source
INCLUDES	:=	$(SOURCES) include
LIBRARIES	:=	
FRAMEWORKS	:=	
LIBDIRS		:=	
LIBS		:=	

CFLAGS		:=	-g -O2 -Wall -c
CXXFLAGS	:=	$(CFLAGS)
CFLAGS		+=	--std=gnu99
LDFLAGS		:=	-bind_at_lead -w -shared

# Generic toolchain settings
PREFIX		:=	arm-apple-darwin9
IPHONE		:=	/var/root/iphone
SDK			:=	$(IPHONE)
INT			:=	install_name_tool
CC			:=	gcc
CXX			:=	g++
LD			:=	$(CC)

LIBDIRS		:=	$(foreach dir,$(LIBDIRS),../$(dir)) \
				$(SDK)/lib
LIBS		+=	$(LIBRARIES) gcc objc stdc++

LDFLAGS		+=	$(foreach dir,$(LIBDIRS),-L$(dir)) \
				$(foreach lib,$(LIBRARIES),-L../lib/$(lib)) \
				$(foreach lib,$(LIBS),-l$(lib)) \
				$(foreach lib,$(FRAMEWORKS),-framework $(lib)) \
				-L$(SDK)/lib/gcc/$(PREFIX)/4.2.1 \
				-F/System/Library/Frameworks \
				-F/System/Library/PrivateFrameworks
INCLUDE		:=	$(foreach dir,$(INCLUDES),-I../$(dir)) \
				-I$(SDK)/include \
				-I$(IPHONE)/Frameworks

ifneq ($(BUILD),$(notdir $(CURDIR)))

export VPATH	:=	$(foreach dir,$(SOURCES),../$(dir)) \
					$(foreach dir,$(DATA),../$(dir))
export DEPSDIR	:=	../$(BUILD)

CPPFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CFILES			:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
MMFILES			:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.mm)))
MFILES			:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.m)))
BINFILES		:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

export LD		:=	$(LD)
export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(MFILES:.m=.o) $(MMFILES:.mm=.o)
export OUTPUT	:=	$(CURDIR)/$(TARGET)

.PHONY: $(BUILD) clean bundle install

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo Clean Binaries...
	@rm -fr $(BUILD) $(OUTPUT) $(OUTPUT).app

bundle:	$(BUILD)
	@rm -rf $(OUTPUT).app
	@mkdir -p $(OUTPUT).app
	@cp $(OUTPUT) $(OUTPUT).app/$(TARGET)
	@cp -r release/* $(OUTPUT).app/
ifneq ($(LIBRARIES),)
	@cp $(foreach lib,$(LIBRARIES),lib/$(lib)/lib$(lib).dylib) $(OUTPUT).app/
endif
	@echo Installed package to $(OUTPUT).app

install: bundle
	@echo Uploading $(TARGET).app to iphone...
	@ssh root@ipod "rm -rf /Applications/$(TARGET).app"
	@scp -r $(OUTPUT).app root@ipod:/Applications/
	@ssh root@ipod "cd /Applications/$(TARGET).app; ldid -S $(TARGET); respring"

else

DEPENDS	:=	$(OFILES:.o=.d)
$(OUTPUT): $(OFILES)
	$(LD) $(LDFLAGS) $(OFILES) -o $(OUTPUT)
ifneq ($(LIBRARIES),)
	$(INT) $(foreach lib,$(LIBRARIES),-change lib$(lib).dylib /Applications/$(notdir $(OUTPUT)).app/lib$(lib).dylib) $(OUTPUT)
endif

%.o : %.cpp
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) $(INCLUDE) -o $@ $<
%.o : %.c
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) $(INCLUDE) -o $@ $<
%.o : %.mm
	$(CXX) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) $(INCLUDE) -o $@ $<
%.o : %.m
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) $(INCLUDE) -o $@ $<

%.jpg.o	:	%.jpg
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

endif
