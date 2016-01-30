SRC :=	${wildcard *.cpp}
HEAD:=	${wildcard *.h}
OBJ	:=	${patsubst %.cpp, %.o, $(SRC)}
OP	:=	Operation.o
MPI	:=	Pi_mpi.o
OMP	:=	Pi_omp.o
TEST:=	Pi_test.o

CC	:=	mpic++
CFLAG := -g -O2 -Wall -fopenmp

all:	test mpi omp

$(OBJ):	%.o: %.cpp $(HEAD)
	$(CC) $< -c -o $@ $(CFLAG)

omp: $(OP) $(OMP)
	$(CC) $^ -o $@.out $(CFLAG)
	
mpi: $(OP) $(MPI)
	$(CC) $^ -o $@.out $(CFLAG)

test: $(OP) $(TEST)
	$(CC) $^ -o $@.out $(CFLAG)

clean:
	rm $(OBJ)

rebuild:	clean all

.PHONY:	all clean rebuild omp mpi debug
