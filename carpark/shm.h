#pragma once

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>   
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <sys/shm.h>
#include <pthread.h>
#include <math.h>

#define SIZE 2920
#define LVLS 5
#define LVL_CAP 20
#define SHARE_NAME "PARKING"

// LPR for entrance
typedef struct lpr_entr {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    char plate[6];
    char padding[2];
} lpr_entr_t;

// boom gate for entrance
typedef struct gate {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    char status;
    char padding[7];
} gate_t;

// information sing for entrance
typedef struct sign_entr {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    char status;
    char padding[7];
} sign_entr_t;


typedef struct entrance {
    lpr_entr_t sensor;
    gate_t gate;
    sign_entr_t sign;
} entrance_t;

typedef struct exit {
    lpr_entr_t sensor;
    gate_t gate;
} exit_t;

typedef struct level {
    lpr_entr_t sensor;
    short tempSensor;
    bool alarm;
    char padding[5];    
} level_t;

typedef struct carpark {
    entrance_t entrances[5];
    exit_t exits[5];
    level_t levels[5];
} carpark_t;

typedef struct shared_memory {
    const char* name;
    int fd;
    carpark_t* data;
} shared_memory_t;

extern shared_memory_t shm;

bool isAlarm;
// void *fireMain();
int getRand(int lower, int upper);
int milli (long msec);
bool create_shared_object( shared_memory_t* shm, const char* share_name );
bool get_shared_object( shared_memory_t* shm, const char* share_name );
void destroy_shared_object( shared_memory_t* shm );
void init_shared_memory (shared_memory_t shm);