CC = g++
LFLAGS = --std=c++11 -O3 -Wall -g -Os #-fno-omit-frame-pointer -fsanitize=address

grad3 : main.o OUTCAR.o POSCAR.o global.o arg_parse.o
	$(CC) -o cgrad main.o OUTCAR.o POSCAR.o global.o arg_parse.o $(LFLAGS)

test : grad3
	unzip ./tests.zip
	./cgrad -o ./test1/OUTCAR -p ./test1/POSCAR -mvu
	./cgrad -o ./test2/OUTCAR -p ./test2/POSCAR -mvu
	./cgrad -o ./test3/OUTCAR -p ./test3/POSCAR -mvu
	./cgrad -o ./test4/OUTCAR -p ./test4/POSCAR -mvu
	./cgrad -o ./test5/OUTCAR -p ./test5/POSCAR -mvu

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
	@rm -r *.o cgrad test1 test2 test3 test4 test5
