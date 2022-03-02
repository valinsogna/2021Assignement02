CXXFLAGS = -Wall -Wextra -std=c99 -O3
CC = gcc

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

executable:
	$(CC) $(CXXFLAGS) -fopenmp main.c kd_tree.c -o main.x 
clean:
	rm -f *.x
	rm -f *.o
	clear