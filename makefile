CC = gcc
CFLAGS = -Wall -pedantic -g -lpthread -lrt
PROGRAMS = rollercoaster

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)
