include $(PHOENIXHOME)/config/clang.mk

CFLAGS += -mno-avx -msse -fno-slp-vectorize -fno-vectorize
