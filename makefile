owr: main.o 
	gcc -Wall -Wextra -std=c99 -g -o owr main.o

main.o: main.c
	gcc -Wall -Wextra -std=c99 -g -c main.c

clean:
	rm -rf *.o owr