CC = icpc
LFLAGS = --std=c++11

grad3 : main.o OUTCAR.o POSCAR.o global.o
	$(CC) -o grad3 main.o OUTCAR.o POSCAR.o global.o $(LFLAGS)

main.o : main.cpp POSCAR.h OUTCAR.h global.h
	$(CC) -c main.cpp $(LFLAGS)
OUTCAR.o : OUTCAR.cpp OUTCAR.h
	$(CC) -c OUTCAR.cpp $(LFLAGS)
POSCAR.o : POSCAR.cpp POSCAR.h
	$(CC) -c POSCAR.cpp $(LFLAGS)
global.o : global.h global.cpp
	$(CC) -c global.cpp $(LFLAGS)
clean :
	rm *.o grad3
