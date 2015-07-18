CFLAGS = -Wall
EXEC = pwnme
SOURCES = board.c data.c eval.c main.c search.c
OBJECTS = ${SOURCES:.c=.o}

${EXEC}: ${OBJECTS}
	cc ${CFLAGS} $^ -o $@
	strip --strip-all ${EXEC}

${EXEC}.exe: ${SOURCES}
	i586-mingw32msvc-cc $^ -o $@ -s

.PHONY: clean zip

clean:
	rm -f ${OBJECTS} ${EXEC} ${EXEC}.exe
	rm -rf out

zip: ${EXEC} ${EXEC}.exe readme.txt
	mkdir -p out
	rm -f out/pwnme.zip
	zip out/pwnme.zip $^
