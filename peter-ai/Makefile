CC = gcc
CFLAGS = 

DEPS = algo.h board.h options.h
OBJ = main.o board.o algo.o

peter-ai: ${OBJ}
	${CC} -o $@ $^ ${CFLAGS}

.PHONY: clean

clean: 
	rm -f *.o

%.o: %.c ${DEPS}
	${CC} -c -o $@ $< ${CFLAGS}

