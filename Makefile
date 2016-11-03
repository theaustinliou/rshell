all: src/rshell.cpp
	g++ src/rshell.cpp -Wall -Werror -ansi -pedantic -o bin/rshell

rshell: rshell
	./rshell
