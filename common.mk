OSTYPE				:=	$(shell echo $$OSTYPE)
CMAKE				:=	$(SHELL) $(BRICKSDIR)/cmake/cmake.sh
CTEST				:=	$(SHELL) $(BRICKSDIR)/cmake/ctest.sh
MAKEIT				:=	$(MAKE) --no-print-directory
TOOLCHAIN_ANDROID	:=	-DCMAKE_TOOLCHAIN_FILE=$(BRICKSDIR)/cmake/toolchain.android.cmake
TOOLCHAIN_IOS		:=	-DCMAKE_TOOLCHAIN_FILE=$(BRICKSDIR)/cmake/toolchain.ios.cmake

all:
	@$(CMAKE) $(CURDIR) $(CURDIR)/build-$(OSTYPE)
	@+$(MAKEIT) -C $(CURDIR)/build-$(OSTYPE)

android:
	@$(CMAKE) $(CURDIR) $(CURDIR)/build-android-armv5 $(TOOLCHAIN_ANDROID) -DNDK_CPU_ARM=y
	@+$(MAKEIT) -C $(CURDIR)/build-android-armv5
	@$(CMAKE) $(CURDIR) $(CURDIR)/build-android-armv7 $(TOOLCHAIN_ANDROID) -DNDK_CPU_ARM_V7A=y -DNDK_CPU_ARM_VFPV3=y
	@+$(MAKEIT) -C $(CURDIR)/build-android-armv7
	@$(CMAKE) $(CURDIR) $(CURDIR)/build-android-x86 $(TOOLCHAIN_ANDROID) -DNDK_CPU_X86=y
	@+$(MAKEIT) -C $(CURDIR)/build-android-x86

ifeq ($(OSTYPE),linux-gnu)
ios:
	@$(CMAKE) $(CURDIR) $(CURDIR)/build-ios-armv6 $(TOOLCHAIN_IOS)
	@+$(MAKEIT) -C $(CURDIR)/build-ios-armv6
else
xcode:
	@$(CMAKE) $(CURDIR) $(CURDIR)/build-$(OSTYPE)-xcode -G Xcode

ios-xcode:
	@$(CMAKE) $(CURDIR) $(CURDIR)/build-ios-xcode $(TOOLCHAIN_IOS) -G Xcode

ios: ios-xcode
	@$(CMAKE) $(CURDIR) $(CURDIR)/build-ios-universal $(TOOLCHAIN_IOS)
	@+$(MAKEIT) -C $(CURDIR)/build-ios-universal
	@$(CMAKE) $(CURDIR) $(CURDIR)/build-ios-simulator $(TOOLCHAIN_IOS) -DIOS_SIMULATOR=y
	@+$(MAKEIT) -C $(CURDIR)/build-ios-simulator

all: xcode
endif

clean:
	@rm -rf build-*

test: all
	@$(CTEST) $(CURDIR)/build-$(OSTYPE)

ifdef EXECUTABLE_NAME
run: all
	@$(CURDIR)/build-$(OSTYPE)/$(EXECUTABLE_NAME)

debug: all
	@gdb $(CURDIR)/build-$(OSTYPE)/$(EXECUTABLE_NAME) -ex run
endif

.PHONY: all xcode clean test run debug android ios ios-xcode
