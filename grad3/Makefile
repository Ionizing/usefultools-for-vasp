CC = clang++
LFLAGS = --std=c++11 -g -Os -fno-omit-frame-pointer -fsanitize=address

grad3 : main.o OUTCAR.o POSCAR.o global.o arg_parse.o
	$(CC) -o grad3 main.o OUTCAR.o POSCAR.o global.o arg_parse.o $(LFLAGS)

main.o : main.cpp POSCAR.h OUTCAR.h global.h
	$(CC) -c main.cpp $(LFLAGS)

OUTCAR.o : OUTCAR.cpp OUTCAR.h
	$(CC) -c OUTCAR.cpp $(LFLAGS)

POSCAR.o : POSCAR.cpp POSCAR.h
	$(CC) -c POSCAR.cpp $(LFLAGS)

global.o : global.h global.cpp
	$(CC) -c global.cpp $(LFLAGS)

arg_parse.o : arg_parse.h arg_parse.cpp
	$(CC) -c arg_parse.cpp $(LFLAGS)

clean :
	touch a.o grad3
	rm *.o grad3
