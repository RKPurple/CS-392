# Rohan Kallur
# I pledge my honor that I have abided by the Stevens Honor System.

all: insertion

insertion: main.o utils.o insertion.o
	gcc main.o utils.o insertion.o 

main.o: main.c
	gcc -c main.c 

insertion.o: insertion.c
	gcc -c insertion.c 

utils.o: utils.c
	gcc -c utils.c 

clean:
	rm -rf *.o insertion
