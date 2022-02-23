CXXFLAGS = -Wall -Wextra -g
CC = gcc

all: main.o kd_tree.o medOfMed.o
	$(CC) main.o kd_tree.o medOfMed.o -o main.x

main.o: main.c kd_tree.h
	$(CC) -c main.c

kd_tree.o: kd_tree.c medOfMed.h kd_tree.h
	$(CC) -c kd_tree.c

medOfMed.o: medOfMed.c medOfMed.h kd_tree.h
	$(CC) -c medOfMed.c
	
mp: 
	$(CC) -fopenmp main.c kd_tree.c -o main.x

clean:
	rm *.x
	rm *.o
	clear
	
	
#gcc -Wall -g -c kd_tree.c -o kd_tree.o
#gcc -Wall -g -c main.c -o main.o