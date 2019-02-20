CC             := g++
CFLAGS_DEBUG   := -std=c++11 -g -Wall -Wextra -fno-omit-frame-pointer -fsanitize=address
CFLAGS_RELEASE := -std=c++11 -O3
CFLAGS_STATIC  := $(CFLAGS_RELEASE) --static
LFLAGS         :=
INCLUDE        := -I./include -I./third_party

INCS     := $(wildcard ./include/*.hpp)
SOURCE   := $(wildcard ./src/*.cc)
OBJS     := $(patsubst %.cc, %.o, $(SOURCE))
TEST_SRC := $(wildcard ./unit_test/*.cc)
TEST_EXE := $(patsubst %.cc, %.out, $(TEST_SRC))

ifeq ($(firstword $(MAKECMDGOALS)), release)
CFLAGS = $(CFLAGS_RELEASE)
else
CFLAGS = $(CFLAGS_RELEASE)
endif

ifeq ($(firstword $(MAKECMDGOALS)), static)
CFLAGS = $(CFLAGS_STATIC)
endif

ifeq ($(firstword $(MAKECMDGOALS)), unit_test)
CFLAGS = $(CFLAGS_DEBUG)
endif

.PHONY: clean release static unit_test test

.SECONDARY: $(OBJS)

release static: cgrad

test: cgrad
	@for i in `ls unit_test | grep -E 'test\d+'` ;	do\
			echo perfoming $$i ... ;			\
			./cgrad -o unit_test/$${i}/OUTCAR ;					\
	done ;

unit_test: clean $(TEST_EXE)
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
