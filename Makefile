SRC :=	${wildcard *.cpp}
HEAD:=	${wildcard *.h}
OBJ	:=	${patsubst %.cpp, %.o, $(SRC)}
OP	:=	Operation
MPI	:=	Pi_mpi
OMP	:=	Pi_omp
TEST:=	Pi_test

MPICC := mpiicc
CC	  := icc
CFLAG := -g -O2 -Wall -fopenmp -offload-attribute-target=mic
MFLAG := -g -O2 -Wall

all: test mpi omp

omp:
	$(CC) $(OMP).cpp -o $(OMP).o $(CFLAG) -c
	$(CC) $(OP).cpp -o $(OP).o $(CFLAG) -c
	$(CC) $(OMP).o $(OP).o -o $(OMP).out $(CFLAG)
	
mpi:
	$(MPICC) $(MPI).cpp -o $(MPI).o -c $(MFLAG)
	$(MPICC) $(OP).cpp -o $(OP).o -c $(MFLAG)
	$(MPICC) $(MPI).o $(OP).o -o $(MPI).out $(MFLAG)

test:
	$(CC) $(TEST).cpp -o $(TEST).o -c $(MFLAG)
	$(CC) $(OP).cpp -o $(OP).o -c $(MFLAG)
	$(CC) $(TEST).o $(OP).o -o $(TEST).out $(MFLAG)

clean:
	rm $(OBJ)

rebuild:	clean all

.PHONY:	all clean rebuild omp mpi test
