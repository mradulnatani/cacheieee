flags = -O2 -Wall -std=c2x
ldflags = 

all: clean cacheieee

cacheieee: cacheieee.o tree.o
	cc ${flags} $^ -o $@ ${ldflags}

cacheieee.o: cacheieee.c tree.h
	cc ${flags} -c $<

tree.o: tree.c tree.h
	cc ${flags} -c $<

clean:
	rm -f *.o cacheieee
