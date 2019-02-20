CC = g++ # gcc 4.9 or upper
CFLAGS = -std=c++11 -O2 -Wall -g -Os
NUMBERS = 1 2

pospot_checker: pospot_checker.cpp
	$(CC) $(CFLAGS) -o pospot_checker pospot_checker.cpp 

clean:
	@rm -rf pospot_checker *.dSYM 2> /dev/null || true

test: pospot_checker
	@$(foreach var,$(NUMBERS), echo running test$(var); cd tests/$(var); ../../pospot_checker; cd ../../;)
