LDFLAGS += --allow-multiple-definition
CFLAGS += -O3  -Iinclude/rt/openmp -w
#-fnoopenmp-use-tls
obj-y += math/
obj-y += common/\
	rand/
obj-y += set_omp_num_threads.o
#EP and MG conflicts with others

#obj-y += \
	 SP/\
	 LU/\
	 IS/\
	 FT/\
         CG/\
         EP/\
         MG/\
        BT/
#--- C class EP and SP can compile ---#

#obj-y += EP/\
	SP/


#--- C class LU CG and MG(MMAP/BUMP ALLOCATOR) can compile ---#

#obj-y += LU/\
	CG/\
	MG/


#--- B class BT can compile only by itself ---#

#obj-y += BT/

#--- B class FT can compile only by itself ---#

obj-y += FT/

#--- C class IS should be configured with large stack size (> 16MB) ---#

#obj-y += IS/






#obj-y+= common/\
	rand/\
	FT/

#obj-y += common/\
	rand/\
	SP/

#obj-y += common/\
         rand/\
	 BT/

#obj-y += common/\
	rand/\
	EP/

#obj-y += common/\
	rand/\
	MG/

#obj-y += CG/\
	common/\
	rand/

#obj-y += LU/\
	common/\
	rand/

#IS
#obj-y += IS/

