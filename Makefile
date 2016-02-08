OP	:=	Operation
MPI	:=	Pi_mpi
OMP	:=	Pi_omp
MIC	:= 	Pi_mic
TEST:=	Pi_test

MPICC := mpic++
CC	  := icc
GCC   := g++
FLAG := -g -Wall -Wextra
MFLAG := $(FLAG) -fopenmp -offload-attribute-target=mic
OFLAG  := $(FLAG) -fopenmp
DFLAG := $(FLAG) -pthread -I.

GTEST_HEADERS = gtest/*.h gtest/internal/*.h
GTEST_SRCS_ = src/*.cc src/*.h $(GTEST_HEADERS)

all:
	@echo "USAGE: \n     make TARGET[mpi, mic, omp, test]"

gtest-all.o : $(GTEST_SRCS_)
	$(GCC) $(DFLAG) -c src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(GCC) $(DFLAG) -c src/gtest_main.cc

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

$(TEST).o $(OP).o:	%.o: %.cpp $(OP).h
	$(GCC) $(DFLAG) -c $<

test:	$(TEST).o $(OP).o gtest_main.a
	$(GCC) $(DFLAG) -lpthread $^ -o $@

mpi: $(MPI).cpp $(OP).cpp $(OP).h
	$(MPICC) $< -c $(FLAG)
	$(MPICC) $(word 2, $^) -c $(FLAG)
	$(MPICC) $(MPI).o $(OP).o $(FLAG) -o $@

mic: $(MIC).cpp $(OP).cpp $(OP).h
	$(CC) $< $(MFLAG) -c
	$(GCC) $(word 2, $^) $(MFLAG) -c
	$(CC) $(MIC).o $(OP).o $(MFLAG) -o $@

omp: $(OMP).cpp $(OP).cpp $(OP).h
	$(GCC) $< $(OFLAG) -c
	$(GCC) $(word 2, $^) $(OFLAG) -c
	$(GCC) $(OMP).o $(OP).o $(OFLAG) -o $@

clean:
	rm -rf *.a *.o *.out

rebuild:	clean all

.PHONY:	all clean rebuild
