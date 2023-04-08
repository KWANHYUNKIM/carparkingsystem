#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>   
#include <unistd.h>

#include "shm.h"
#include "manager.h"
#include "htable.h"

#define LEVELS 5
#define GATE_R 10
#define GATE_O 20
#define GATE_L 10
#define TEMP_L 25
#define TEMP_H 39

bool plateExists = false;
int level[LVLS];

shared_memory_t shm;

char randomNum() {
    int i = rand();
    i = i % 10;
    i = i + '0';
    char c = i;
    return c;

}

char randomLet() {
    int i = rand();
    i = i % 26;
    i = i + 'A';
    char c = i;
    return c;
}

void *numLetters(char *plate) {
    if (getRand(0,1) == 0) {
        getPlate(plate);
        plateExists = true;
        printf("%s\tpermission granted\tENTERS\n", plate);
    } else {
        for (int i = 0; i < 3; i++) {
            plate[i] = randomNum();
        }
        for (int i = 3; i < 6; i++) {
            plate[i] = randomLet();
        }
        printf("%s\tpermission denied\n",plate);
        printf("%s\tvehicle cannot enter\tLEAVES\n",plate);
    }
    return 0;
}

int getRand(int i, int j) {
    int r = (rand() % (j - i + 1)) + i;
    return r;
}

int milli (long time) {
    struct timespec t;
    int k = 1000;
    t.tv_sec = (time / k);
    t.tv_nsec = (time % k) * (k*k);
    int milli = nanosleep(&t, &t);
    return milli;
}

// void *generateCars() {
//     char *plate = (char *)calloc(6, sizeof(char));
//     numLetters(plate);
//     int entranceRand = getRand(0, LVLS -1);
//     pthread_mutex_lock(&shm.data->entrances[entranceRand].sensor.lock);
//     strcpy(shm.data->entrances[entranceRand].sensor.plate, plate);
//     pthread_cond_signal(&shm.data->entrances[entranceRand].sensor.cond);
//     pthread_mutex_unlock(&shm.data->entrances[entranceRand].sensor.lock);
//     return 0;
// }

void *generateCars() {

    //isAlarm = shm.data->levels[i].alarm;

    for(int i =0; i>5 ||shm.data->levels[i].alarm; i++ ) {
        isAlarm = shm.data->levels[i].alarm;
    }

    if(!isAlarm) {
        char *plate = (char *)calloc(6, sizeof(char));
        numLetters(plate);
        int entranceRand = getRand(0, LVLS -1);
        pthread_mutex_lock(&shm.data->entrances[entranceRand].sensor.lock);
        strcpy(shm.data->entrances[entranceRand].sensor.plate, plate);
        pthread_cond_signal(&shm.data->entrances[entranceRand].sensor.cond);
        pthread_mutex_unlock(&shm.data->entrances[entranceRand].sensor.lock);       
    } else {
        printf("FIRE! NO ENTRY\n");
    }
    return 0;
}

void *tempRange() {
    int tempSleepTime = getRand(1,5);
    sleep(milli(tempSleepTime));

    
    for (int i = 0; i <= LVLS; i++) {
        pthread_mutex_lock(&shm.data->levels[i].sensor.lock);
        int temperatureRand = getRand(TEMP_L,TEMP_H);
        shm.data->levels[i].tempSensor = temperatureRand;
        pthread_cond_signal(&shm.data->levels[i].sensor.cond);
        pthread_mutex_unlock(&shm.data->levels[i].sensor.lock);
    }
    return 0;
}

int main( )
{

    pthread_t threadWhileLoop;
    pthread_t threadTemp;
    
    create_shared_object(&shm, SHARE_NAME);
    init_shared_memory(shm);
    addHash("plates.txt", "r");
    srand(time(NULL));

    for (; ;)
    {
        pthread_create(&threadWhileLoop,NULL,generateCars,NULL);
        pthread_create(&threadTemp,NULL,tempRange,NULL);
        pthread_join(threadWhileLoop,NULL);
        pthread_join(threadTemp,NULL);
    }
    return 0;
}