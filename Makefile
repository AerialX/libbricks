all:
	@make --no-print-directory -f Makefile.pc
	@make --no-print-directory -f Makefile.test

clean:
	@make --no-print-directory -f Makefile.pc clean
	@make --no-print-directory -f Makefile.test clean

run: all
	@make --no-print-directory -f Makefile.test run

.PHONY: all clean test
