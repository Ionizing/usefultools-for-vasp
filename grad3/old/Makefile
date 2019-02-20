CC = g++
LFLAGS = --std=c++11 -O3 -Wall -g -Os #-fno-omit-frame-pointer -fsanitize=address
NUMBERS = 1 2 3 4 5 6

grad3 : main.o OUTCAR.o POSCAR.o global.o arg_parse.o
	$(CC) -o cgrad main.o OUTCAR.o POSCAR.o global.o arg_parse.o $(LFLAGS)

test : grad3 $(TEST_FILES)
	unzip -o ./tests.zip
	$(foreach var,$(NUMBERS), echo running test$(var); ./cgrad -mvu -o ./test$(var)/OUTCAR -p ./test$(var)/POSCAR;)

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
	-@rm -r *.o *.gch \~ cgrad test1 test2 test3 test4 test5 test6 test7 2> /dev/null || true
