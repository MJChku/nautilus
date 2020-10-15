include $(PHOENIXHOME)/config/gcc.mk

CC = $(LLVMHOME)/bin/clang
RANLIB = /bin/true

CFLAGS += -mrtm -flto -Wl,-plugin-opt=save-temps -funroll-loops #-DFAULTINJECTION
