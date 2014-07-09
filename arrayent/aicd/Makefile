#
# Copyright (C) 2012, Arrayent Inc.
# All Rights Reserved
#
#
# File: application Makefile
#
# Purpose: This is a toplevel makefile for compiling the
# 	 applications and utilities in subfolders
#
include ./config

SUBDIRS =  zamport linux app
 
ifeq "$(BUILD_SOURCE)" "Y"
all:
	@mkdir -p bin lib
	@set -e; for i in $(SUBDIRS); do $(MAKE) LTIB_ROOTFS=$(LTIB_ROOTFS) -C $$i ; done
	@echo
	@echo "***********************************************"
	@echo "** Build complete			    **"
	@echo "***********************************************"

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
else
all:
	@echo "Nothing to do"

clean:
	@echo "Nothing to do"
endif

#mcm_install_sysroot:
#	@echo "Nothing to do"

#mcm_install_rfs:
#	mkdir -p $(DESTDIR)/usr/sbin
#	install bin/aicd  $(DESTDIR)/usr/sbin
