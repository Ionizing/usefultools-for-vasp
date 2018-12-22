CC = g++
CFLAGS = -std=c++11 -O3 -lopenblas -L/usr/local/opt/openblas/lib

main: test.cpp BinIO.hpp
	$(CC) -o test test.cpp $(CFLAGS)
