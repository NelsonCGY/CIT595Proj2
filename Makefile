CC = c++
ARGS = -Wall

all: main

main:
	$(CC) -pthread -o main $(ARGS) main.cpp ReadUSB.cpp -I /path/to/boost_1_60_0/

clean: 
	rm -rf main *.o

