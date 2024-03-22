all:
	gcc -c -o parser.o parser.c
	g++ -o eshell *.cpp parser.o -std=c++17
