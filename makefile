CC = gcc
CFLAGS = -Wall -pedantic -g -lpthread
PROGRAMS = rollercoaster

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)
