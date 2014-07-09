#
# File: libaic build Makefile
#
# Purpose: To build Arrayent library related code
#

include ./config

BINDIR  = ./lib

LIBANAME = libaic.a
LIBSOBASE = libaic.so
LIBSOVERSION = 1.0.0
LIBSONAME = $(LIBSOBASE).$(LIBSOVERSION) 

LIBA_OBJS	:= client.o 
LIBSO_OBJS	:= client.pic.o 

#SHELL = /bin/bash

COMPC = $(CC)
COMPCC = $(CC)
ARCH = $(AR)
ARFLAGS = crs

ifeq ($(DEBUG), Y)
CFLAGS += -DDEBUG=1 -g
#OPTIMIZE = -O2
else
OPTIMIZE = -O2
endif

INCLUDES = -I. -I../aicd/zamport/include/ -I../aicd/linux/include/ -I../aicd/app/ 

LIBS = -lrt -lpthread -lc

dirs:
	@mkdir -p $(BINDIR)

%.o : %.c
	$(COMPC) $(CFLAGS) $(OPTIMIZE) $(INCLUDES) -o $@ -c $<

%.pic.o : %.c
	$(COMPC) -fPIC $(CFLAGS) $(OPTIMIZE) $(INCLUDES) -o $@ -c $<


$(BINDIR)/$(LIBANAME): $(LIBA_OBJS)
	$(ARCH) $(ARFLAGS) $@ $(LIBA_OBJS) 

$(BINDIR)/$(LIBSONAME): $(LIBSO_OBJS)
	$(COMPC) -shared -Wl,-soname,$(LIBSOBASE).1 -o $@ $(LIBSO_OBJS) $(LIBS) 

ifeq "$(BUILD_SOURCE)" "Y"
all: dirs $(BINDIR)/$(LIBANAME) $(BINDIR)/$(LIBSONAME)

clean:
	@rm -fv *.o
	@rm -f $(OBJS)
	@rm -f $(BINDIR)/$(LIBANAME)
	@rm -f $(BINDIR)/$(LIBSONAME)
else
all:
	@echo "Nothing to do"

clean:
	@echo "Nothing to do"

endif
mcm_install_sysroot:
	install lib/libaic.a  $(MCM_SYSROOT)/usr/lib/
	install lib/libaic.so*  $(MCM_SYSROOT)/usr/lib/
	install aic.h  $(MCM_SYSROOT)/usr/include/

mcm_install_rfs:
	@echo "Nothing to do"

################################################################################

