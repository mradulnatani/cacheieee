flags=-O2 -Wall -std=c2x

all: clean cacheme

cacheme : cacheme.o
	cc ${flags} $^ -o $@ ${ldflags}


cacheme.o : cacheme.c
	cc ${flags} -c $^

clean: 
	rm -f *.o cacheme