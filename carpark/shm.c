#include "shm.h"

bool create_shared_object( shared_memory_t* shm, const char* share_name ) {
    shm_unlink(share_name);
    shm -> name = share_name;
    int shm_fd = shm_open(share_name, O_CREAT | O_RDWR, 0666);
    shm -> fd = shm_fd; 

    if (shm_fd < 0) {
        shm -> data = NULL;
        return false;
    }

    if (ftruncate(shm->fd, SIZE) != 0){
        shm->data = NULL;
        return false;
    }

    carpark_t *carpark = mmap(NULL, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = carpark;
    if (shm->data == MAP_FAILED)
    {
        return false;
    }

    return true;
}

bool create_shared_object_RW( shared_memory_t* shm, const char* share_name ) {
    shm_unlink(share_name);
    shm->name = share_name;

    if ((shm->fd = shm_open(share_name, O_CREAT | O_RDWR, 0666)) < 0){
        shm->data = NULL;
        return false;
    }

    if (ftruncate(shm->fd,SIZE) == -1){
        shm->data = NULL;
        return false;
    }

    if ((shm->data = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm->fd, 0)) == (void *)-1)
    {
        return false;
    }
    return true;
}

bool get_shared_object( shared_memory_t* shm, const char* share_name ) {

    shm->name = share_name;
    int shm_fd = shm_open(share_name, O_RDWR, 0);
    shm -> fd = shm_fd;
    if (shm_fd < 0) 
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    carpark_t *map = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    shm -> data = map;

    if (map == MAP_FAILED)
    {
        shm -> fd = -1;
        shm -> data = false;
        return false;
    }

    return true;
}   


bool create_shared_object_R( shared_memory_t* shm, const char* share_name ) {
    shm->name = share_name;

    if ((shm->fd = shm_open(share_name, O_RDWR, 0)) < 0) {
        shm->data = NULL;
        return false;
    }

    if ((shm->data = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm->fd, 0)) == (void *)-1) {
        return false;
    }
    return true;
}

void destroy_shared_object( shared_memory_t* shm ) {
    munmap(&shm, 48);
    shm_unlink(shm -> name);
    shm -> fd = -1;
    shm -> data = NULL; 
}

void init_shared_memory (shared_memory_t shm){

    pthread_mutexattr_t attrmutex;
    pthread_condattr_t attrcond;

    pthread_mutexattr_init(&attrmutex);
    pthread_condattr_init(&attrcond);
    
    for (int i = 0; i < 5; i++) {
        pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&shm.data->entrances[i].sensor.lock, &attrmutex);
        pthread_mutex_init(&shm.data->levels[i].sensor.lock, &attrmutex);
        pthread_mutex_init(&shm.data->exits[i].sensor.lock, &attrmutex);

        pthread_condattr_setpshared(&attrcond, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(&shm.data->entrances[i].sensor.cond, &attrcond);
        pthread_cond_init(&shm.data->levels[i].sensor.cond, &attrcond);
        pthread_cond_init(&shm.data->exits[i].sensor.cond, &attrcond);
    }
}