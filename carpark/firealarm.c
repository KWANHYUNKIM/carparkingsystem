#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "manager.h"
#include "shm.h"

int alarm_active = 0;

shared_memory_t shm;

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t alarm_condvar = PTHREAD_COND_INITIALIZER;

#define LEVELS 5
#define ENTRANCES 5
#define EXITS 5
#define MEDIAN_WINDOW 5
#define TEMPCHANGE_WINDOW 30
#define EVA 100

bool isAlarm = false;

struct tempnode {
	int temperature;
	struct tempnode *next;
};

struct tempnode *deletenodes(struct tempnode *templist, int after)
{
	if (templist->next) {
		templist->next = deletenodes(templist->next, after - 1);
	}
	if (after <= 0) {
		free(templist);
		return NULL;
	}
	return templist;
}
int compare(const void *first, const void *second)
{
	return *((const int *)first) - *((const int *)second);
}

int milli (long time) {
    struct timespec t;
    int k = 1000;
    t.tv_sec = (time / k);
    t.tv_nsec = (time % k) * (k*k);
    int milli = nanosleep(&t, &t);
    return milli;
}

void *tempmonitor(void *level)
{
	sleep(milli(250));

	struct tempnode *templist = NULL;
	struct tempnode *newtemp;
	struct tempnode *medianlist = NULL;
	struct tempnode *oldesttemp;
	int count = 0;
	int mediantemp = 0;
	int hightemps = 0;
	int *tempLevel = (int *)level;

	if (shm.data->levels[*tempLevel].tempSensor == 0)
	{
		return 0;
	}
	
	for (;;) {
		sleep(milli(50));

		/* Add temperature to beginning of linked list */
		newtemp = malloc(sizeof(struct tempnode));
		newtemp->temperature = shm.data->levels[*tempLevel].tempSensor; 
		newtemp->next = templist;
		templist = newtemp;
		
		/* Delete nodes after 5th */
		deletenodes(templist, MEDIAN_WINDOW);
		
		/* Count nodes */
		count = 0;
		for (struct tempnode *t = templist; t != NULL; t = t->next) {
			count++;
		}

		if (count == MEDIAN_WINDOW) { /* Temperatures are only counted once we have 5 samples */
			int *sorttemp = malloc(sizeof(int) * MEDIAN_WINDOW);
			count = 0;
			for (struct tempnode *t = templist; t != NULL; t = t->next) {
				count++;
				sorttemp[count] = t->temperature;
			}
			qsort(sorttemp, MEDIAN_WINDOW, sizeof(int), compare);
			mediantemp = sorttemp[(MEDIAN_WINDOW - 1) / 2];

			printf("Median temp: %d\n",mediantemp);
			
			/* Add median temp to linked list */
			newtemp = malloc(sizeof(struct tempnode));
			newtemp->temperature = mediantemp;
			newtemp->next = medianlist; 
			medianlist = newtemp;
			
			/* Delete nodes after 30th */
			deletenodes(medianlist, TEMPCHANGE_WINDOW);
			
			/* Count nodes */
			count = 0;
			hightemps = 0;
			
			for (struct tempnode *t = medianlist; t != NULL; t = t->next) { 
				/* Temperatures of 58 degrees and higher are a concern */
				if (t->temperature >= 58) {
					hightemps++;
				}
				/* Store the oldest temperature for rate-of-rise detection */
				oldesttemp = t; 
				count++;
			}
			
			if (count == TEMPCHANGE_WINDOW) {
				/*	If 90% of the last 30 temperatures are >= 58 degrees,
					this is considered a high temperature. Raise the alarm */
				if (hightemps >= TEMPCHANGE_WINDOW * 0.9){
					alarm_active = 1;
					isAlarm = true;
					for(int i=0; i<5; i++)
					{
						shm.data->levels[i].alarm = isAlarm;
					}
					
				}
				/* 	If the newest temp is >= 8 degrees higher than the oldest
					temp (out of the last 30), this is a high rate-of-rise.
					Raise the alarm */
				if (templist->temperature - oldesttemp->temperature >= 8)
				{
					alarm_active = 1;
					isAlarm = true;
					for(int i=0; i<5; i++)
					{
						shm.data->levels[i].alarm = isAlarm;
					}
				}
			}
		}
		usleep(2000);
	}
	return 0;
}

void *openboomgate(void *arg) {
	int *entrance = (int *) arg;
	pthread_mutex_lock(&shm.data->entrances[*entrance].gate.lock);
	pthread_mutex_lock(&shm.data->exits[*entrance].gate.lock);
	for (;;) {

		for (int i = 0; i < LEVELS; i++) {
			shm.data->entrances[i].gate.status = 'O';
		}

		for (int i = 0; i < LEVELS; i++) {
			shm.data->exits[i].gate.status = 'O';
		}

	}
	pthread_mutex_unlock(&shm.data->entrances[*entrance].gate.lock);
	pthread_mutex_unlock(&shm.data->exits[*entrance].gate.lock);
}

int main() {
	if (!get_shared_object(&shm, SHARE_NAME)) {
        printf("Memory access not initialised\n");
    }
	pthread_t *threads = malloc(sizeof(pthread_t) * LEVELS);
	for (int i = 0; i < 5; i++) {
            if(pthread_create(threads + i, NULL, tempmonitor, &i) !=0){
                perror("Thread not created");
                return 1;
            }
            printf("Temp monitor thread %d initialised\n,", i);	
	}
	for (;;) {
		if (alarm_active) {
			for (int i = 0; i < LEVELS; i++) {
				shm.data->levels[i].alarm = true;
				strcpy(shm.data->exits[i].sensor.plate, shm.data->levels[i].sensor.plate);
			}
			pthread_t *boomgatethreads = malloc(sizeof(pthread_t) * (ENTRANCES + EXITS));
			for (int i = 0; i < ENTRANCES; i++) {
					
				if(pthread_create(boomgatethreads + i, NULL, openboomgate, &i) !=0){
					perror("Thread entrance not created");
					return 1;
				}
				printf("Gates thread %d initialised\n,", i);
			}
			for (int i = 0; i < EXITS; i++) {
				pthread_create(boomgatethreads + ENTRANCES + i, NULL, openboomgate, &i);
			}
			for (;;) {
				sleep(milli(EVA));
				char *evacmessage = "EVACUATE ";
				for (int i = 0; i < LEVELS; i++) {
					shm.data->entrances[i].gate.status = 'O';
				}
				for (int i = 0; i < LEVELS; i++) {
					shm.data->exits[i].gate.status = 'O';
				}
				for (char *p = evacmessage; *p != '\0'; p++) {
					for (int i = 0; i < ENTRANCES; i++) {
						sign_entr_t *sign = &shm.data->entrances[i].sign;
						pthread_mutex_lock(&sign->lock);
						sign->status = *p;
						pthread_mutex_unlock(&sign->lock);
					}
					usleep(20000);
				}
			}
			for (int i = 0; i < LEVELS; i++) {
				pthread_join(threads[i], NULL);
			}
		}
	}

	return 0;
}
