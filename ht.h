#ifndef  HT_H_
#define  HT_H_

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>


#define FNV_offset 0x811c9dc5
#define FNV_prime_1 713724233 
#define FNV_prime_1a 8473966301

#define LOAD_FACTOR 0.7
#define INIT_SIZE   32

typedef unsigned int uint;


typedef struct ht_item{
    char* key;
    char* value;
} ht_item;
typedef  struct ht_table{
    uint size;
    uint count;
    ht_item** items;
} ht_table;


static ht_item DEAD_ITEM = {.key=NULL,.value=NULL};

static ht_item* ht_create_item(const char* key,const char* value);
static void ht_free_item(ht_item* item);
ht_table* ht_create_table();
void ht_free_table(ht_table* table);

void ht_print_table(const ht_table* table);

static uint hash_func(const char* key,uint times,uint size);
static uint fnv_1(const char* key);
static uint fnv_1a(const char* key);
void ht_insert(ht_table* table,const char* key, const char* value);

char* ht_get(ht_table* table,const char* key);

void ht_resize(ht_table* table);

static ht_item* ht_create_item(const char* key,const char* value) {

    ht_item* item = (ht_item*) malloc(sizeof(ht_item)); 
    
    item->key   = strdup(key);
    item->value = strdup(value);

    return item;
}
static void ht_free_item(ht_item* item) {
    if(!item || item == &DEAD_ITEM) return; 

    free((char*)item->key);
    free((char*)item->value);
    free(item);
}

ht_table* ht_create_sized_table(uint size) {
    assert(size > 0 && "size must be a positive integer");

    ht_table* table= (ht_table*) malloc(sizeof(ht_table));
    table->count = 0;
    table->size = size;
    table->items = (ht_item**) malloc(sizeof(ht_item*) * size);
    for (size_t i = 0; i < size; i++) {
        table->items[i] = NULL;
    }
    

    return table;
}

ht_table* ht_create_table() { 
    return ht_create_sized_table(INIT_SIZE);
}

void ht_free_table(ht_table* table) {
    for (size_t i = 0; i < table->size; i++) {
        ht_free_item(table->items[i]);
    }
    free(table->items);
    table->size  = 0;
    table->count = 0;

    free(table);
    table = NULL;
    
}


void ht_print_table(const ht_table* table) {
   printf("{\n");
   for(size_t i = 0; i < table->size;i++) {
       if(table->items[i] != NULL && table->items[i] != &DEAD_ITEM) {
            printf("  %04ld   %s : %s\n",i,table->items[i]->key,table->items[i]->value);
       }
   }
   printf("}\n");
}

static uint hash_func(const char* key,uint times,uint size) {
    return (fnv_1(key) + (times * fnv_1a(key) + 1)) % size;
}

static uint fnv_1(const char* key) {
    uint i = 0;
    uint hashed =  FNV_offset;
    while(key[i]) {
        hashed = hashed * FNV_prime_1;
        hashed = hashed ^ key[i];
        i++;

    }
    return hashed;
}
static uint fnv_1a(const char* key) {
    uint i = 0;
    uint hashed = FNV_offset;
    while(key[i]) {
        hashed = hashed ^ key[i];
        hashed = hashed * FNV_prime_1a; 
        i++;
    }
    return hashed;
}

void ht_insert(ht_table* table,const char* key, const char* value) {
    uint times = 0;
    uint i = 0; 

    if((float)table->count/table->size  >= LOAD_FACTOR) {
        ht_resize(table);
    }
    ht_item** dead_item = NULL;
    do {
        i = hash_func(key,times,table->size);
        times++;

        if(table->items[i] == &DEAD_ITEM){
            dead_item = &table->items[i];
        }
        
        // replacing value
        if(table->items[i] != NULL &&  table->items[i] != &DEAD_ITEM && strcmp(table->items[i]->key,key) == 0) {
            free(table->items[i]->value);
            table->items[i]->value = strdup(value);
            return;
        }
    } while(table->items[i] != NULL);
    


    ht_item* item = ht_create_item(key,value);

    if(dead_item != NULL) {
        *dead_item = item;
    } else {
        table->items[i] = item;
    }
    table->count++;
}

char* ht_get(ht_table* table,const char* key) {
    uint times = 0;
    uint i = 0; 
    do {
        i = hash_func(key,times,table->size);
        times++;
        if(table->items[i] == NULL) {
            return NULL; 
        }
    } while(table->items[i] == &DEAD_ITEM || strcmp(table->items[i]->key,key) != 0);

    return table->items[i]->value;
}


void ht_remove(ht_table* table,const char* key) { 
    uint times = 0;
    uint i = 0; 
    do {
        i = hash_func(key,times,table->size);
        times++;
        if(table->items[i] == NULL) {
            return;
        }
    } while(strcmp(table->items[i]->key,key) != 0);

    ht_free_item(table->items[i]);
    table->items[i] = &DEAD_ITEM;
    
    table->count++;
}

void ht_resize(ht_table* table) {
    uint i = 0;
    uint counted = 0;
    
    ht_table* t = ht_create_sized_table(table->size * 2);
    while(i < table->size && counted < table->count ) {
        if(table->items[i] != NULL && table->items[i] != &DEAD_ITEM) {
            ht_insert(t,table->items[i]->key,table->items[i]->value);
            counted++;
        }
        i++;
    }

     
    ht_item** tmp_items = table->items;

    table->items = t->items;
    table->size= t->size;

    t->items = tmp_items;
    t->size= t->size / 2;
    ht_free_table(t);
}

#endif
