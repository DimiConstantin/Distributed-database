CC=gcc
CFLAGS=-Wall -Wextra -g #-fsanitize=address,undefined

LOAD=load_balancer
SERVER=server
CACHE=lru_cache
UTILS=utils
DATA = data_structures
# Add new source file names here:
# EXTRA=<extra source file name>

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(CACHE).o $(UTILS).o  $(DATA).o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(CACHE).o: $(CACHE).c $(CACHE).h
	$(CC) $(CFLAGS) $^ -c

$(UTILS).o: $(UTILS).c $(UTILS).h
	$(CC) $(CFLAGS) $^ -c

$(DATA).o: $(DATA).c $(DATA).h
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch
