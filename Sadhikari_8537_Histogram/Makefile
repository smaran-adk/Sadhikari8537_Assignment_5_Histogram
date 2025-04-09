CC = gcc
CFLAGS = -Wall -pthread
TARGETS = bin/dp1 bin/dp2 bin/dc

all: $(TARGETS)

bin/dp1: src/dp1.c include/histo.h
	$(CC) $(CFLAGS) src/dp1.c -o bin/dp1 -lrt -pthread

bin/dp2: src/dp2.c include/histo.h
	$(CC) $(CFLAGS) src/dp2.c -o bin/dp2 -lrt -pthread

bin/dc: src/dc.c include/histo.h
	$(CC) $(CFLAGS) src/dc.c -o bin/dc -lrt -pthread

clean:
	rm -f bin/dp1 bin/dp2 bin/dc
