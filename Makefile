CC=g++
CFLAGS=-I.

client: client.cpp requests.cpp helpers.cpp json.hpp
		$(CC) -o client client.cpp requests.cpp helpers.cpp json.hpp -fno-stack-protector -Wall
		
run: client
		./client

clean:
		rm -f *.o client
