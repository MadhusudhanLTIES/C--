TOP := $(PWD)

include	$(TOP)/.config

SUBDIRS=\
    aicd \
    libaic \
    client \

all:
	for i in $(SUBDIRS); do \
		$(P) $(MAKE) -C $$i all CC="$(CC)" LD="$(LD)" AR=$(AR) LTIB_ROOTFS=$(LTIB_ROOTFS); \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

clean:
	for i in $(SUBDIRS); do \
		$(P) $(MAKE) -C $$i clean; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

#mcm_install_sysroot:
#	for i in $(SUBDIRS); do \
#		$(P) $(MAKE) -C $$i mcm_install_sysroot MCM_SYSROOT=$(MCM_SYSROOT) ; \
#		if [ $$? != 0 ]; then exit 1; fi; \
#	done

#mcm_install_rfs:
#	for i in $(SUBDIRS); do \
#		$(P) $(MAKE) -C $$i mcm_install_rfs DESTDIR=$(DESTDIR) CROSS_COMPILE=$(CROSS_COMPILE) ; \
#		if [ $$? != 0 ]; then exit 1; fi; \
#	done
