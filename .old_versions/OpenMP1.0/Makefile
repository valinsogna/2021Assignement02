CXXFLAGS = -Wall -Wextra -std=c99
CC = gcc

all: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable

executable:
	$(CC) -fopenmp main_mp.c kd_tree_mp.c medOfMed_mp.c -o main_mp.x $(CXXFLAGS)

# all: executable

# debug: CXXFLAGS += -DDEBUG -g
# debug: executable

# executable: main_mp.o kd_tree_mp.o medOfMed_mp.o
# 	$(CC) main_mp.o kd_tree_mp.o medOfMed_mp.o -o main_mp.x

# main_mp.o: main_mp.c kd_tree_mp.h
# 	$(CC) -fopenmp kd_tree_mp.c main_mp.c $(CXXFLAGS)

# kd_tree_mp.o: kd_tree_mp.c medOfMed_mp.h kd_tree_mp.h
# 	$(CC) -fopenmp medOfMed_mp.c kd_tree_mp.c $(CXXFLAGS)

# medOfMed_mp.o: medOfMed_mp.c medOfMed_mp.h kd_tree_mp.h
# 	$(CC) -fopenmp medOfMed_mp.c $(CXXFLAGS)
	
# mp: 
# 	gcc -fopenmp main_mp.c kd_tree_mp.c medOfMed_mp.c -o main_mp.x

clean:
	rm -f *.x
	rm -f *.o
	clear