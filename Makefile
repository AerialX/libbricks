OSTYPE	:= $(shell echo $$OSTYPE)

all:
	@sh ./cmake/cmake.sh build-$(OSTYPE)
	@$(MAKE) --no-print-directory -C build-$(OSTYPE) -j8

android:
	@sh ./cmake/cmake.sh buildandroid-arm -DCMAKE_TOOLCHAIN_FILE=$(PWD)/cmake/toolchain.android.cmake -DARM_TARGET=armeabi
	@$(MAKE) --no-print-directory -C buildandroid-arm -j8
	@sh ./cmake/cmake.sh buildandroid-armv7a -DCMAKE_TOOLCHAIN_FILE=$(PWD)/cmake/toolchain.android.cmake -DARM_TARGET="armeabi-v7a with VFPV3"
	@$(MAKE) --no-print-directory -C buildandroid-armv7a -j8
	@sh ./cmake/cmake.sh buildandroid-x86 -DCMAKE_TOOLCHAIN_FILE=$(PWD)/cmake/toolchain.android.cmake -DARM_TARGET=x86
	@$(MAKE) --no-print-directory -C buildandroid-x86 -j8

ifeq ($(OSTYPE),linux-gnu)
ios:
	@sh ./cmake/cmake.sh buildios -DCMAKE_TOOLCHAIN_FILE=$(PWD)/cmake/toolchain.ios.cmake
	@$(MAKE) --no-print-directory -C buildios -j8
else
ios:
	@sh ./cmake/cmake.sh buildios -DCMAKE_TOOLCHAIN_FILE=$(PWD)/cmake/toolchain.ios.cmake
	@$(MAKE) --no-print-directory -C buildios -j8
	@sh ./cmake/cmake.sh buildios-simulator -DCMAKE_TOOLCHAIN_FILE=$(PWD)/cmake/toolchain.ios.cmake -DIOS_SIMULATOR=true
	@$(MAKE) --no-print-directory -C buildios-simulator -j8
endif

clean:
	@rm -rf build*

test: all
	@sh ./cmake/ctest.sh build-$(OSTYPE)

run: all
	@./build-$(OSTYPE)/sample

.PHONY: all clean test run android ios
