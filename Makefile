Libs = lib/parseLib3.o lib/randomLib.o lib/shellSort.o
execs=$(filter %.c, $(shell ls))
CC=gcc

all: $(patsubst %.c, %.o, $(execs))


lib/%.o: %.c
	$(CC) -x c -c $^ -o $@

%.o: $(Libs) %.c
	$(CC) $^ -o $*.o -lm

$(Libs): %.o : %.c
	$(CC) -x c -c $^ -o $@
clean:
	-rm *.o
	-rm lib/*.o


