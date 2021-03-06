CC = c++
ARGS = -Wall

all: clean server main

server:
	$(CC) -pthread -o server $(ARGS) JSONServer.cpp ReadUSB.cpp -I ./boost_1_60_0/

main:
	$(CC) -pthread -o main $(ARGS) main.cpp ReadUSB.cpp -I ./boost_1_60_0/

clean: 
	rm -rf server main *.o

