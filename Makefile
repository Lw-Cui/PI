SRC :=	${wildcard	*.cpp}
TAR :=	${patsubst %.cpp, %.out, $(SRC)}

CC	:=	g++
CFLAG :=	-g -Wall -O2

all:	$(TAR)

$(TAR):	%.out:	%.cpp
	$(CC) $^ -o %@ $(CFLAG)

clean:
	rm $(TAR) -rf

rebuild:	clean all

.PHONY:	all clean rebuild
