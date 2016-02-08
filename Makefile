SRC :=	${wildcard *.cpp}
HEAD:=	${wildcard *.h}
OP	:=	Operation.o
MPI	:=	Pi_mpi.o
OMP	:=	Pi_omp.o
TEST:=	Pi_test.o
CC	:=	mpic++
CFLAG := -g -Wall -Wextra -pthread -fopenmp -I.

all: test mpi omp

GTEST_HEADERS = gtest/*.h gtest/internal/*.h
GTEST_SRCS_ = src/*.cc src/*.h $(GTEST_HEADERS)

gtest-all.o : $(GTEST_SRCS_)
	$(CC) $(CFLAG) -c src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CC) $(CFLAG) -c src/gtest_main.cc

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

$(MPI) $(OMP) $(TEST) $(OP): %.o: %.cpp $(HEAD) $(GTEST_HEADERS)
	$(CC) $< -c -o $@ $(CFLAG)

omp: $(OP) $(OMP)
	$(CC) $^ -o $@.out $(CFLAG)
	
mpi: $(OP) $(MPI)
	$(CC) $^ -o $@.out $(CFLAG)

test:	$(TEST) $(OP) gtest_main.a
	$(CC) $(CFLAG) -lpthread $^ -o $@.out

clean:
	rm -rf *.a *.o *.out

rebuild:	clean all

.PHONY:	all clean rebuild omp mpi test
