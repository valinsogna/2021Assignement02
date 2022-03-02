CXXFLAGS = -Wall -Wextra -std=c99
CC = gcc

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

executable:
	$(CC) $(CXXFLAGS) -fopenmp main_mp.c kd_tree_mp.c -o main_mp.x 
clean:
	rm -f *.x
	rm -f *.o
	clear