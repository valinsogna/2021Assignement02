CXXFLAGS = -Wall -Wextra -std=c99 -O3
CC = mpicc

all: executable

debug: CXXFLAGS += -DDEBUG
debug: executable

executable:
	$(CC) $(CXXFLAGS) main.c kd_tree.c utils.c -o main.x -lm
clean:
	rm -f *.x
	rm -f *.o
	clear