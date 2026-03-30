owr: main.o node.o udp.o general.o stdin_msg.o udp_msg.o tcp_msg.o tcp.o
	gcc -Wall -Wextra -std=gnu99 -g -o owr main.o node.o udp.o general.o stdin_msg.o udp_msg.o tcp_msg.o tcp.o

main.o: main.c node.h general.h
	gcc -Wall -Wextra -std=gnu99 -g -c main.c

general.o: general.c general.h node.h udp.h stdin_msg.h udp_msg.h tcp_msg.h
	gcc -Wall -Wextra -std=gnu99 -g -c general.c	

stdin_msg.o: stdin_msg.c stdin_msg.h node.h udp.h tcp.h
	gcc -Wall -Wextra -std=gnu99 -g -c stdin_msg.c

tcp_msg.o: tcp_msg.c tcp_msg.h node.h tcp.h
	gcc -Wall -Wextra -std=gnu99 -g -c tcp_msg.c

udp_msg.o: udp_msg.c udp_msg.h node.h udp.h
	gcc -Wall -Wextra -std=gnu99 -g -c udp_msg.c

node.o: node.c node.h udp.h
	gcc -Wall -Wextra -std=gnu99 -g -c node.c

udp.o: udp.c udp.h
	gcc -Wall -Wextra -std=gnu99 -g -c udp.c

tcp.o: tcp.c tcp.h
	gcc -Wall -Wextra -std=gnu99 -g -c tcp.c			

clean:
	rm -rf *.o owr