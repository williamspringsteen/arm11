CC = gcc
CFLAGS = -Wall -Werror -pedantic -std=c99 -g

assemble: assemblerMain.o assemblerUtils.o encoder.o tuple.o

assemblerMain.o: tuple.c tuple.h encoder.c encoder.h assemblerUtils.c assemblerUtils.h

assemblerUtils.o: assemblerUtils.c assemblerUtils.h encoder.c encoder.h

encoder.o: assemblerUtils.c assemblerUtils.h encoder.c encoder.h tuple.c tuple.h

tuple.o: tuple.c tuple.h

clean:
	$(RM) assemble *.o *~

.PHONY: clean
