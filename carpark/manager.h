#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define LEVELS 5
#define ENTRANCES 5
#define EXITS 5
#define SHM_NAME "PARKING"
#define PRICE 5/100
#define T_START 10
#define T_EXIT 10
#define GATE_R 10
#define GATE_O 20
#define GATE_L 10
#define WAIT_L 10
#define WAIT_U 10000

double lprRev;