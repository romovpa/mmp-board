CC = gcc
CFLAGS = 

DEPS = 
OBJ = main.o board.o

peter-ai: ${OBJ}
	${CC} -o $@ $^ ${CFLAGS}

.PHONY: clean

clean: 
	rm -f *.o

%.o: %.c ${DEPS}
	${CC} -c -o $@ $< ${CFLAGS}

