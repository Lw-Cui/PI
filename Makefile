SRC :=	${wildcard *.cpp}
HEAD:=	${wildcard *.h}
OP	:=	Operation
MPI	:=	Pi_mpi
OMP	:=	Pi_omp
MIC	:= 	Pi_mic
TEST:=	Pi_test

MPICC := mpic++
CC	  := icc
GCC   := g++
FLAG := -g -Wall -Wextra -pthread -I.
MFLAG := $(FLAG) -fopenmp -offload-attribute-target=mic
GFLAG  := $(FLAG) -fopenmp

GTEST_HEADERS = gtest/*.h gtest/internal/*.h
GTEST_SRCS_ = src/*.cc src/*.h $(GTEST_HEADERS)

all:
	@echo "USAGE: \n     make TARGET[mpi, mic, omp, test]"

gtest-all.o : $(GTEST_SRCS_)
	$(GCC) $(FLAG) -c src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(GCC) $(FLAG) -c src/gtest_main.cc

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

$(TEST).o $(OP).o:	%.o: %.cpp $(OP).h
	$(GCC) $(FLAG) -c $<

test:	$(TEST).o $(OP).o gtest_main.a
	$(GCC) $(FLAG) -lpthread $^ -o $@.out

mpi: $(MPI).cpp $(OP).cpp $(OP).h
	$(MPICC) $(MPI).cpp -o $(MPI).o -c $(FLAG)
	$(MPICC) $(OP).cpp -o $(OP).o -c $(FLAG)
	$(MPICC) $(MPI).o $(OP).o -o $@.out $(FLAG)

mic: $(MIC).cpp $(OP).cpp $(OP).h
	$(CC) $(MIC).cpp -o $(MIC).o $(MFLAG) -c
	$(CC) $(OP).cpp -o $(OP).o $(MFLAG) -c
	$(CC) $(MIC).o $(OP).o $(MFLAG) -o $@.out 

omp: $(OMP).cpp $(OP).cpp $(OP).h
	$(GCC) $(OMP).cpp -o $(OMP).o $(GFLAG) -c
	$(GCC) $(OP).cpp -o $(OP).o $(GFLAG) -c
	$(GCC) $(OMP).o $(OP).o -o $@.out $(GFLAG)

clean:
	rm -rf *.a *.o *.out

rebuild:	clean all

.PHONY:	all clean rebuild omp mpi test
