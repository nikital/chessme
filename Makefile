CFLAGS = -g -Wall
EXEC = chessme
SOURCES = board.c data.c eval.c main.c search.c
OBJECTS = ${SOURCES:.c=.o}

${EXEC}: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o ${EXEC}

.PHONY: clean zip

clean:
	rm ${OBJECTS} ${EXEC}
