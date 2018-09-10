# 
#
#

CPP = clang++
LFLAGS = --std=c++11 -O3 -Wall -g -Os -fno-omit-frame-pointer -fsanitize=address

main: POSCAR.o main.o
	$(CPP) -o main POSCAR.o main.o $(LFLAGS)

POSCAR.o: POSCAR.h POSCAR.cpp
	$(CPP) -c POSCAR.cpp $(LFLAGS)

main.o: main.cpp
	$(CPP) -c main.cpp  $(LFLAGS)

clean:
	touch main a.o
	rm main *.o

NUMBERS = 1 2 3 4

test: main
	$(foreach var,$(NUMBERS),./main ./tests/test_$(var).txt;)
