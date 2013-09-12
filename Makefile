#
# Runs make in all subdirectories containing Makefile file
#
# Author: Adam Strzelecki <adam.strzelecki@uj.edu.pl>
#
# @see src/Makefile for main source build settings
#

subdirs := $(patsubst %/,%,$(dir $(wildcard **/Makefile)))
# use terminal colors if in terminal
bold := \033[1m
none := \033[0m
 
all:
clean:
clean-all:
%::
	@for dir in $(subdirs); do \
		[ -t 1 ] && printf "$(bold)## %s$(none)\n" "$$dir" || echo "## $$dir"; \
		make --no-print-directory -C $$dir $*; \
	done
