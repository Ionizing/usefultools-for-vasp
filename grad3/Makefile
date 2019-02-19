CC := c++
CFLAGS := -std=c++11 -g -Wall -Wextra -fno-omit-frame-pointer -fsanitize=address
LFLAGS := 
INCLUDE := -I./include -I./third_party

INCS := $(wildcard ./include/*.hpp)
SOURCE := $(wildcard ./src/*.cc)
OBJS := $(patsubst %.cc, %.o, $(SOURCE))
TEST_SRC := $(wildcard ./unit_test/*.cc)
TEST_EXE := $(patsubst %.cc, %.out, $(TEST_SRC))

.PHONY: clean all

.SECONDARY: $(OBJS)

all: $(TEST_EXE) cgrad

test: $(TEST_EXE)
	@cd unit_test; 										\
	for i in `ls | grep -E 'out$$'` ; \
		do														 	\
			echo 'executing ' $$i ' ...';	\
			./$$i ;	  										\
		done ;

./src/%.o: ./src/%.cc $(INCS)
	$(CC) -o $@ -c $< $(INCLUDE) $(CFLAGS)

./unit_test/%.out: ./unit_test/%.cc $(OBJS) $(INCS)
	$(CC) -o $@ $< $(OBJS) $(INCLUDE) $(CFLAGS) $(LFLAGS)

cgrad: main.cc $(OBJS) $(INCS)
	$(CC) -o $@ $< $(OBJS) $(INCLUDE) $(CFLAGS) $(LFLAGS)

clean:
	@rm -rf $(OBJS) $(TEST_EXE) ./unit_test/*.dSYM \
		./unit_test/POSCAR* ./unit_test/animate.molden \
		./unit_test/poscar_frames ./cgrad ./cgrad.dSYM
