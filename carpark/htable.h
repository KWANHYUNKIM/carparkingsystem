//#pragma once

// #include <inttypes.h>
// #include <stdbool.h>
// #include <stdio.h> 
// #include <stdlib.h>
//#include <string.h>

typedef struct item item_t;
struct item
{
    char *key;
    int value;
    item_t *next;
};

typedef struct htab htab_t;
struct htab
{
    item_t **buckets;
    size_t size;
};

htab_t h;

// size_t djb_hash(char *s);
// size_t htab_index(htab_t *h, char *key);
// item_t *htab_bucket(htab_t *h, char *key);
// item_t *htab_find(htab_t *h, char *key);

// bool htab_init(htab_t *h, size_t n);
// bool htab_add(htab_t *h, char *key, int value);
bool getPlate( char *plate );
// void htab_print(htab_t *h);
// void htab_destroy(htab_t *h);
void addHash( const char * filename, const char * mode );
// void item_print(item_t *i);