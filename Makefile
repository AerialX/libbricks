all:
	@sh ./cmake build
	@$(MAKE) --no-print-directory -C build -j8

android:
	@sh ./cmake buildandroid-arm -DCMAKE_TOOLCHAIN_FILE=$(PWD)/toolchain.android.cmake -DARM_TARGET=armeabi
	@$(MAKE) --no-print-directory -C buildandroid-arm -j8
	@sh ./cmake buildandroid-armv7a -DCMAKE_TOOLCHAIN_FILE=$(PWD)/toolchain.android.cmake -DARM_TARGET="armeabi-v7a with VFPV3"
	@$(MAKE) --no-print-directory -C buildandroid-armv7a -j8
	@sh ./cmake buildandroid-x86 -DCMAKE_TOOLCHAIN_FILE=$(PWD)/toolchain.android.cmake -DARM_TARGET=x86
	@$(MAKE) --no-print-directory -C buildandroid-x86 -j8

ios:
	@sh ./cmake buildios-arm -DCMAKE_TOOLCHAIN_FILE=$(PWD)/toolchain.ios.cmake -DARMEABI=true -DARMEABI_V7A=false
	@$(MAKE) --no-print-directory -C buildios-arm -j8
	@sh ./cmake buildios-armv7a -DCMAKE_TOOLCHAIN_FILE=$(PWD)/toolchain.ios.cmake -DARMEABI=true -DARMEABI_V7A=true -DARM_NEON=true
	@$(MAKE) --no-print-directory -C buildios-armv7a -j8

clean:
	@rm -rf build*

test: all
	@sh ./ctest build

run: all
	@./build/sample

.PHONY: all clean test run android ios
