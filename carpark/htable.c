/* Obtained via QUT Blackboard Practical 3 */

#include "shm.h"
#include "htable.h"

#define SIZE_PLATES 100
size_t buckets = SIZE_PLATES;

void item_print(item_t *i)
{
    printf("key=%s value=%d", i->key, i->value);
}

// Initialise a new hash table with n buckets.
// pre: true
// post: (return == false AND allocation of table failed)
//       OR (all buckets are null pointers)
bool htab_init(htab_t *h, size_t n)
{
    h->size = n;
    h->buckets = (item_t **)calloc(n, sizeof(item_t *));
    return h->buckets != 0;
}

size_t djb_hash(char *s)
{
    size_t hash = 5381;
    int c;
    while ((c = *s++) != '\0')
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

size_t htab_index(htab_t *h, char *key)
{
    return djb_hash(key) % h->size;
}

item_t *htab_bucket(htab_t *h, char *key)
{
    return h->buckets[htab_index(h, key)];
}

item_t *htab_find(htab_t *h, char *key)
{
    for (item_t *i = htab_bucket(h, key); i != NULL; i = i->next)
    {
        if (strcmp(i->key, key) == 0)
        {
            return i;
        }
    }
    return NULL;
}

// Add a key with value to the hash table.
// pre: htab_find(h, key) == NULL
// post: (return == false AND allocation of new item failed)
//       OR (htab_find(h, key) != NULL)
bool htab_add(htab_t *h, char *key, int value)
{
    // allocate new item
    item_t *newhead = (item_t *)malloc(sizeof(item_t));
    if (newhead == NULL)
    {
        return false;
    }
    newhead->key = key;
    newhead->value = value;
    size_t bucket = htab_index(h, key);
    newhead->next = h->buckets[bucket];
    h->buckets[bucket] = newhead;
    return true;
}

// Print the hash table.
// pre: true
// post: hash table is printed to screen
void htab_print(htab_t *h)
{
    printf("hash table with %ld buckets\n", h->size);
    for (size_t i = 0; i < h->size; ++i)
    {
        printf("bucket %ld: ", i);
        if (h->buckets[i] == NULL)
        {
            printf("empty\n");
        }
        else
        {
            for (item_t *j = h->buckets[i]; j != NULL; j = j->next)
            {
                item_print(j);
                if (j->next != NULL)
                {
                    printf(" -> ");
                }
            }
            printf("\n");
        }
    }
}

// Destroy an initialised hash table.
// pre: htab_init(h)
// post: all memory for hash table is released
void htab_destroy(htab_t *h)
{
    // free linked lists
    for (size_t i = 0; i < h->size; ++i)
    {
        item_t *bucket = h->buckets[i];
        while (bucket != NULL)
        {
            item_t *next = bucket->next;
            free(bucket);
            bucket = next;
        }
    }
    // free buckets array
    free(h->buckets);
    h->buckets = NULL;
    h->size = 0;
}

void addHash( const char * filename, const char * mode ) {
    if (!htab_init(&h, buckets))
    {
        printf("failed to initialise hash table\n");
        //return EXIT_FAILURE;
    }
    //return 0;
    
    FILE* fp;
    int i = 0;
    fp = fopen(filename, mode);
    if (fp == NULL) {
        perror("Failed: ");
        return;
    }

    char charBuff[256];
    while (fgets(charBuff, sizeof charBuff, fp)) {
        charBuff[strcspn(charBuff, "\n")] = 0;
        char *plate = (char *)malloc(sizeof(char)*100);  
        strcpy(plate, charBuff);
        htab_add(&h, plate, i);
        i++;
    }
    fclose(fp);
    //return;
}

bool getPlate ( char *plate ) {
    int i = getRand( 1, 100 );
    int j = 0;
    char charBuff[256]; 
    
    FILE *fp = fopen("plates.txt", "r");
    if ( fp == NULL) {
        return false;
    }

    while (fgets(charBuff, sizeof charBuff, fp) != NULL)
    {
        charBuff[strcspn(charBuff, "\n")] = 0;
        if (j == i)
        {
            fclose(fp);
            strcpy(plate, charBuff);

            return true;
        } else {
            j++;
        }
    }
    return false;
}