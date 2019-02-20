CC = g++
CFLAGS = -std=c++11 -O3

main: test.cpp BinIO.hpp
	$(CC) -o test BinIO.cpp test.cpp $(CFLAGS)

clean: test
	rm test
