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

typedef struct ht_item_cstr{
    char* key;
    char* value;
} ht_item_cstr;
typedef  struct ht_table_cstr{
    uint size;
    uint count;
    ht_item_cstr** items;
} ht_table_cstr;

static ht_item_cstr DEAD_ITEM_cstr= {.key=NULL,.value=NULL};


static ht_item_cstr* ht_create_item_cstr(const char* key,const char* value);
static void ht_free_item_cstr(ht_item_cstr* item);
ht_table_cstr* ht_create_table_cstr();
void ht_free_table_cstr(ht_table_cstr* table);
void ht_print_table_cstr(const ht_table_cstr* table);
void ht_insert_cstr(ht_table_cstr* table,const char* key, const char* value);
char* ht_get_cstr(ht_table_cstr* table,const char* key);
void ht_resize_cstr(ht_table_cstr* table);
    
static ht_item_cstr* ht_create_item_cstr(const char* key,const char* value) {

    ht_item_cstr* item = (ht_item_cstr*) malloc(sizeof(ht_item_cstr)); 
        
    item->key   = strdup(key);
    item->value = strdup(value);

    return item;
}
static void ht_free_item_cstr(ht_item_cstr* item) {
    if(!item || item == &DEAD_ITEM_cstr) return; 

    free((char*)item->key);
    free((char*)item->value);
    free(item);
}

static ht_table_cstr* ht_create_sized_table_cstr(uint size) {
    assert(size > 0 && "size must be a positive integer");
 
    ht_table_cstr* table= (ht_table_cstr*) malloc(sizeof(ht_table_cstr));
    table->count = 0;
    table->size = size;
    table->items = (ht_item_cstr**) malloc(sizeof(ht_item_cstr*) * size);
    for (size_t i = 0; i < size; i++) {
        table->items[i] = NULL;
    }
    return table;
}

ht_table_cstr* ht_create_table_cstr() { 
    return ht_create_sized_table_cstr(INIT_SIZE);
}

void ht_free_table_cstr(ht_table_cstr* table) {
    for (size_t i = 0; i < table->size; i++) {
        ht_free_item_cstr(table->items[i]);
    }
    free(table->items);
    table->size  = 0;
    table->count = 0;

    free(table);
    table = NULL;
    
}


void ht_print_table_cstr(const ht_table_cstr* table) {
   printf("{\n");
   for(size_t i = 0; i < table->size;i++) {
       if(table->items[i] != NULL && table->items[i] != &DEAD_ITEM_cstr) {
            printf("  %04ld   %s : %s\n",i,table->items[i]->key,table->items[i]->value);
       }
   }
   printf("}\n");
}

void ht_insert_cstr(ht_table_cstr* table,const char* key, const char* value) {
    uint times = 0;
    uint i = 0; 

    if((float)table->count/table->size  >= LOAD_FACTOR) {
        ht_resize_cstr(table);
    }
    ht_item_cstr** dead_item = NULL;
    do {
        i = hash_func(key,times,table->size);
        times++;

        if(table->items[i] == &DEAD_ITEM_cstr){
            dead_item = &table->items[i];
        }
        
        // replacing value
        if(table->items[i] != NULL &&  table->items[i] != &DEAD_ITEM_cstr && strcmp(table->items[i]->key,key) == 0) {
            free(table->items[i]->value);
            table->items[i]->value = strdup(value);
            return;
        }
    } while(table->items[i] != NULL);
    


    ht_item_cstr* item = ht_create_item_cstr(key,value);

    if(dead_item != NULL) {
        *dead_item = item;
    } else {
        table->items[i] = item;
    }
    table->count++;
}

char* ht_get_cstr(ht_table_cstr* table,const char* key) {
    uint times = 0;
    uint i = 0; 
    do {
        i = hash_func(key,times,table->size);
        times++;
        if(table->items[i] == NULL) {
            return NULL; 
        }
    } while(table->items[i] == &DEAD_ITEM_cstr || strcmp(table->items[i]->key,key) != 0);

    return table->items[i]->value;
}


void ht_remove_cstr(ht_table_cstr* table,const char* key) { 
    uint times = 0;
    uint i = 0; 
    do {
        i = hash_func(key,times,table->size);
        times++;
        if(table->items[i] == NULL) {
            return;
        }
    } while(strcmp(table->items[i]->key,key) != 0);

    ht_free_item_cstr(table->items[i]);
    table->items[i] = &DEAD_ITEM_cstr;
    
    table->count++;
}

void ht_resize_cstr(ht_table_cstr* table) {
    uint i = 0;
    uint counted = 0;
    
    ht_table_cstr* t = ht_create_sized_table_cstr(table->size * 2);
    while(i < table->size && counted < table->count ) {
        if(table->items[i] != NULL && table->items[i] != &DEAD_ITEM_cstr) {
            ht_insert_cstr(t,table->items[i]->key,table->items[i]->value);
            counted++;
        }
        i++;
    }

     
    ht_item_cstr** tmp_items = table->items;

    table->items = t->items;
    table->size= t->size;

    t->items = tmp_items;
    t->size= t->size / 2;
    ht_free_table_cstr(t);
}

#undef FNV_offset  
#undef FNV_prime_1 
#undef FNV_prime_1a
#undef LOAD_FACTOR  
#undef INIT_SIZE   
#endif
