#include "manager.h"
#include "shm.h"

int capacity[LEVELS];
char allowedPlates[100][7];

bool isAlarm = false;

int carCapacity = 0;

shared_memory_t shm;
pthread_mutex_t mutexBilling;

typedef struct lprStart {
	char plateNumber[6];
	int entrance;
} lprStart_t;

int getRand(int i, int j) {
    int r = (rand() % (j - i + 1)) + i;
    return r;
}


void readFile(char *filename){
    FILE* file = fopen(filename, "r");
    int i = 0;
    while (fgets(allowedPlates[i], 10, file)) {
        allowedPlates[i][strlen(allowedPlates[i]) - 1] = '\0';
        i++;
    }
}

void billing(char *plateNumber, double carRev) {
    pthread_mutex_lock(&mutexBilling);
    FILE *fp = fopen("billing.txt", "a");
    fprintf(fp, "%s $%.2f\n", plateNumber, carRev);
    fclose(fp);
    pthread_mutex_unlock(&mutexBilling);
}

int milli (long time) {
    struct timespec t;
    int k = 1000;
    t.tv_sec = (time / k);
    t.tv_nsec = (time % k) * (k*k);
    int milli = nanosleep(&t, &t);
    return milli;
}

int timeIn() {
    int time = getRand(WAIT_L, WAIT_U);
	int res = T_START + time + T_EXIT;
	sleep(milli(T_START));
    sleep(milli(milli(time)));
	sleep(milli(T_EXIT));
    return res;
}

void printStart() {
    printf("╔");
    for (int i = 0; i < 50; i++) {
        printf("═");
    }
    printf("╗\n");
}

void printEnd() {
    printf("╚");
    for (int i = 0; i < 50; i++) {
        printf("═");
    }
    printf("╝\n");
}

void printRevenue() {
    printf("║ REVENUE\n");
    printf("║\n");
    printf("║ $%.2f\n", lprRev);
}

void printCapacity() {
    printf("║ LVLs  SPACES  CAPACITY\n");
    printf("║\n");
    printf("║ %d\t%d\t%d / %d\n", LVLS, LVL_CAP, carCapacity, LVL_CAP * LVLS);
}

void printEntrance() {
    printf("║ ENTER\n");
    printf("║\n");
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
        printf("║ L%d\tcar: %s\tgate: %c\t\tsign: %c\n", i+1, shm.data->entrances[i].sensor.plate, shm.data->entrances[i].gate.status, shm.data->entrances[i].sign.status);
        pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
    }
}

void printLevel() {
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&shm.data->levels[i].sensor.lock);
        printf("║\n");
        printf("║ L%d\t\t%s\n║ capacity:\t%d / %d\n║ temp:\t\t%hu°C\n", i+1, shm.data->levels[i].sensor.plate, capacity[i], LVL_CAP, shm.data->levels[i].tempSensor);
        pthread_mutex_unlock(&shm.data->levels[i].sensor.lock);
    }
}

void printExit() {
    printf("║ EXIT\n");
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&shm.data->exits[i].sensor.lock);
        printf("║\n");
        printf("║ L%d\t%s\n║ gate: %c\n", i+1, shm.data->exits[i].sensor.plate, shm.data->exits[i].gate.status);
        pthread_mutex_unlock(&shm.data->exits[i].sensor.lock);
    }
}

bool firealarm(bool isAlarm) {
    for(int i = 0 ; i < 5; i ++)
    {
        isAlarm = shm.data->levels[i].alarm;
        
        if(isAlarm == true){
            return true;
        }
    }
    return false;
}

void *display() {
    for (;;) {
        if (firealarm(isAlarm) == true) {
            carCapacity = 0;
            for (int i = 0; i < LEVELS; i++) {
                capacity[i] = 0;
            }
        }
        fflush(0);
        // revenue
        printStart();
        printRevenue();
        printEnd();
        // capacity
        printStart();
        printCapacity();
        printEnd();
        // entrance
        printStart();
        printEntrance();
        printEnd();
        // level
        printStart();
        printLevel();
        printEnd();
        // exit
        printStart();
        printExit();
        printEnd();
        // clear
        sleep(milli(50));
        system("clear");

        }
    return 0;
}

void openBoomgate(int entrance) {
        char gateStatus[4] = {'R', 'O', 'L', 'C'};
        pthread_mutex_lock(&shm.data->entrances[entrance].gate.lock);
        shm.data->entrances[entrance].gate.status = gateStatus[0];
        sleep(milli(GATE_R));
        pthread_mutex_unlock(&shm.data->entrances[entrance].gate.lock);
        pthread_mutex_lock(&shm.data->entrances[entrance].gate.lock);
        shm.data->entrances[entrance].gate.status = gateStatus[1];
        sleep(milli(GATE_O));
        pthread_mutex_unlock(&shm.data->entrances[entrance].gate.lock);
        pthread_mutex_lock(&shm.data->entrances[entrance].gate.lock);   
        shm.data->entrances[entrance].gate.status = gateStatus[2];
        sleep(milli(GATE_L));
        pthread_mutex_unlock(&shm.data->entrances[entrance].gate.lock);
        pthread_mutex_lock(&shm.data->entrances[entrance].gate.lock);
        shm.data->entrances[entrance].gate.status = gateStatus[3];
        pthread_mutex_unlock(&shm.data->entrances[entrance].gate.lock);
}

char entranceIn() {
    int i = rand();
    i = i % 5;
    i = i + '1';
    char c = i;
    return c;
}

void *entryStart(void *p) {
	lprStart_t *lprCar = p;
    int level = lprCar->entrance;
	int exit = lprCar->entrance;
    char *plateNumber = lprCar->plateNumber;
    double billingRev;
    char gateStatus[4] = {'R', 'O', 'L', 'C'};
    capacity[level]++;
    carCapacity++;

    billingRev = timeIn() *PRICE;
    lprRev += billingRev;
    billing(plateNumber, billingRev);
    
    pthread_mutex_lock(&shm.data->levels[level].sensor.lock);
    strcpy(shm.data->levels[level].sensor.plate , plateNumber);
    pthread_mutex_unlock(&shm.data->levels[level].sensor.lock);
	pthread_mutex_lock(&shm.data->exits[exit].gate.lock);
	strcpy(shm.data->exits[exit].sensor.plate, plateNumber);	
	shm.data->exits[exit].gate.status = gateStatus[0];
	sleep(milli(GATE_R));
	pthread_mutex_unlock(&shm.data->exits[exit].gate.lock);
	pthread_mutex_lock(&shm.data->exits[exit].gate.lock);
	shm.data->exits[exit].gate.status = gateStatus[1];
	sleep(milli(GATE_O));
	pthread_mutex_unlock(&shm.data->exits[exit].gate.lock);
	pthread_mutex_lock(&shm.data->exits[exit].gate.lock);
	shm.data->exits[exit].gate.status = gateStatus[2];
	sleep(milli(GATE_L));
	pthread_mutex_unlock(&shm.data->exits[exit].gate.lock);
	pthread_mutex_lock(&shm.data->exits[exit].gate.lock);
	shm.data->exits[exit].gate.status = gateStatus[3];
	pthread_mutex_unlock(&shm.data->exits[exit].gate.lock);
    
    
    capacity[level]--;
    carCapacity--;

	free(lprCar);

	return 0;
}

void lprSend(char *plateNumber, int entrance) {
	pthread_t carsThread;
	lprStart_t *lprCar = malloc(sizeof(lprStart_t));
	strcpy(lprCar->plateNumber, plateNumber);
	lprCar->entrance = entrance;
	pthread_mutex_init(&mutexBilling, NULL);
	pthread_create(&carsThread, NULL, entryStart, (void *)lprCar);
}

void *lprIn(int entrance){
    int i = entrance;
    char sign;
    while (firealarm(isAlarm) == false) { 
       
        for (int j = 0; j < 100; j++) {
            pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
            pthread_cond_wait(&shm.data->entrances[i].sensor.cond, &shm.data->entrances[i].sensor.lock);
            int returnVal = strcmp(shm.data->entrances[i].sensor.plate, allowedPlates[j]);
            pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
        if (returnVal != 0) { 
            //sign = 'X';
            if (returnVal <= 0) {
                sign = 'X';
            } else {
                sign = entranceIn();
            }
            pthread_mutex_lock(&shm.data->entrances[i].sign.lock);
            shm.data->entrances[i].sign.status = sign;
            pthread_mutex_unlock(&shm.data->entrances[i].sign.lock);
            } else {
                sign = entranceIn();          
                pthread_mutex_lock(&shm.data->entrances[i].sign.lock);
                shm.data->entrances[i].sign.status = sign;
                pthread_mutex_unlock(&shm.data->entrances[i].sign.lock);

                openBoomgate(i);
                char *plate = (char *)calloc(6, sizeof(char));
                pthread_mutex_lock(&shm.data->entrances[i].sensor.lock);
                plate = shm.data->entrances[i].sensor.plate;
                lprSend(plate, i);   
                pthread_mutex_unlock(&shm.data->entrances[i].sensor.lock);
            }
        }
    }
return 0;
}


int main(void) {
    lprRev = 0;
    readFile("plates.txt");
    
    if (!get_shared_object(&shm, SHARE_NAME)) {
        printf("memory access failed\n");
    }

    // init capacity 
    for (int i = 1 ; i<= LEVELS; i++)
    {
        capacity[i] = 0;
    }

    pthread_t disPlay , ent_th[5];

    for (;;) {   

        pthread_create(&disPlay,NULL,display, NULL);

        for(int i = 0; i < 5; i++) {
            long l = i;
            if(pthread_create(ent_th + i, NULL, (void *)&lprIn, (void*)l) !=0){
                perror("Failed to create entrance thread");
                return 1;
            }
            //printf("Thread %d has started\n,", i);
        }
        


        
        for(int i = 0; i < 5; i++){
            if(pthread_join(ent_th[i], NULL) !=0){
                perror("Failed to create entrance thread");
                return 2;
            }
            //printf("Thread %d has finished execution\n,", i);
        }

        pthread_join(disPlay,NULL);

    }

    return 0;
}