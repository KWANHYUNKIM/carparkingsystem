CC = gcc
OUTPUT = -o
EXTRA = -Wall -Wextra -Werror -pthread -lrt

all: simulator manager firealarm

simulator : simulator.c
	$(CC) $(OUTPUT) simulator simulator.c htable.c shm.c $(EXTRA)

manager : manager.c
	$(CC) $(OUTPUT) manager manager.c shm.c $(EXTRA)

firealarm : firealarm.c
	$(CC) $(OUTPUT) firealarm firealarm.c shm.c $(EXTRA)

clean:
	rm -f simulator manager firealarm *.o

.PHONY: all clean