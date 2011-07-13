all:
	@$(MAKE) --no-print-directory -f Makefile.pc.mk
	@$(MAKE) --no-print-directory -C test

android:
	@ndk-build APP_BUILD_SCRIPT=Android.mk NDK_PROJECT_PATH=. NDK_APP_OUT=build -j8

clean:
	@$(MAKE) --no-print-directory -f Makefile.pc.mk clean
	@$(MAKE) --no-print-directory -C test clean

run: all
	@$(MAKE) --no-print-directory -C test run

debug: all
	@$(MAKE) --no-print-directory -C test debug

.PHONY: all clean test run debug
