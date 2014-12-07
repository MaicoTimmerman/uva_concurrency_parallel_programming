PROG = reduce-max
NVCC = nvcc
CC = g++
#CU_FLAGS= -O3 -g -deviceemu
CU_FLAGS = -g --ptxas-options=-v -arch compute_20 -code sm_20
CC_FLAGS = -m64 -Wall

CU_SOURCES = reduce-max.cu
CC_SOURCES = timer.cc

CU_OBJECTS = $(CU_SOURCES:%.cu=%.o)
CU_PTX = $(CU_SOURCES:%.cu=%.ptx)
CC_OBJECTS = $(CC_SOURCES:%.cc=%.o)

%.o: %.cu
	$(NVCC) $(CU_FLAGS) -c $< -o $@

%.o: %.cc
	$(CC) $(CC_FLAGS) -c $< -o $@

%.ptx: %.cu
	$(NVCC) $(CU_FLAGS) --ptx $< -o $@

$(PROG): $(CU_OBJECTS) $(CC_OBJECTS)
	$(NVCC) $^ -o $@

ptx: $(CU_PTX)

run: $(PROG)
	prun -np 1 -native '-l gpu=GTX480' ./$(PROG) 1048576 1024

clean:
	rm -f *.o $(PROG) *~
