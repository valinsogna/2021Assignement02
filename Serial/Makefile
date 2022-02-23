CXXFLAGS = -Wall -Wextra -std=c99
CC = gcc

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

executable: main.o kd_tree.o medOfMed.o
	$(CC) main.o kd_tree.o medOfMed.o -o main.x

main.o: main.c kd_tree.h
	$(CC) -c main.c $(CXXFLAGS)

kd_tree.o: kd_tree.c medOfMed.h kd_tree.h
	$(CC) -c kd_tree.c $(CXXFLAGS)

medOfMed.o: medOfMed.c medOfMed.h kd_tree.h
	$(CC) -c medOfMed.c $(CXXFLAGS)

clean:
	rm -f *.x
	rm -f *.o
	clear