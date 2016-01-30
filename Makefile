SRC :=	${wildcard *.cpp}
HEAD:=	${wildcard *.h}
OBJ	:=	${patsubst %.cpp, %.o, $(SRC)}
OP	:=	Operation
MPI	:=	Pi_mpi
OMP	:=	Pi_omp
TEST:=	Pi_test

MPICC := mpiicc
CC	  := icc
GCC   := g++
CFLAG := -g -O2 -Wall -fopenmp -offload-attribute-target=mic
MFLAG := -g -O2 -Wall
GFLAG  := -g -O2 -Wall -fopenmp

all:
	@echo "USAGE: \n     make TARGET[mpi, mic, omp, test]"

mpi:
	$(MPICC) $(MPI).cpp -o $(MPI).o -c $(MFLAG)
	$(MPICC) $(OP).cpp -o $(OP).o -c $(MFLAG)
	$(MPICC) $(MPI).o $(OP).o -o $@.out $(MFLAG)

mic:
	$(CC) $(OMP).cpp -o $(OMP).o $(CFLAG) -c
	$(CC) $(OP).cpp -o $(OP).o $(CFLAG) -c
	$(CC) $(OMP).o $(OP).o $(CFLAG) -o $@.out 

omp:
	$(GCC) $(OMP).cpp -o $(OMP).o $(GFLAG) -c
	$(GCC) $(OP).cpp -o $(OP).o $(GFLAG) -c
	$(GCC) $(OMP).o $(OP).o -o $@.out $(GFLAG)

test:
	$(GCC) $(TEST).cpp -o $(TEST).o -c $(MFLAG)
	$(GCC) $(OP).cpp -o $(OP).o -c $(MFLAG)
	$(GCC) $(TEST).o $(OP).o -o $@.out $(MFLAG)

clean:
	rm *.o
	rm *.out

.PHONY:	all clean rebuild omp mpi test
