bunch_dir = ../bunch/
hashtable_dir = ../hashtable/
holdall_dir = ../holdall/
wordstruct_dir = ../wordstruct/
CC = gcc
CFLAGS = -std=c18 \
  -Wall -Wconversion -Werror -Wextra -Wfatal-errors -Wpedantic -Wwrite-strings \
  -O2 \
  -I$(bunch_dir) -I$(hashtable_dir) -I$(holdall_dir) -I$(wordstruct_dir) -DNDEBUG
LDFLAGS =
vpath %.h $(bunch_dir) $(hashtable_dir) $(holdall_dir) $(wordstruct_dir)
vpath %.c $(bunch_dir) $(hashtable_dir) $(holdall_dir) $(wordstruct_dir)
objects = bunch.o hashtable.o holdall.o wordstruct.o main.o
executable = ws

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

bunch.o: bunch.c bunch.h
hashtable.o: hashtable.c hashtable.h bunch.h
holdall.o: holdall.c holdall.h bunch.h
wordstruct.o: wordstruct.c wordstruct.h
main.o: main.c hashtable.h holdall.h
