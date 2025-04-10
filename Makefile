flags = -O2 -Wall -std=c2x
ldflags = 

all: clean cacheme

cacheme: cacheme.o tree.o
	cc ${flags} $^ -o $@ ${ldflags}

cacheme.o: cacheme.c tree.h
	cc ${flags} -c $<

tree.o: tree.c tree.h
	cc ${flags} -c $<

clean:
	rm -f *.o cacheme
