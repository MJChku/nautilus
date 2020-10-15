include $(PHOENIXHOME)/config/gcc.mk

CFLAGS += -mno-avx -mno-mmx -mno-sse2 -mno-sse3 -mno-sse4 -msse -fno-tree-vectorize
