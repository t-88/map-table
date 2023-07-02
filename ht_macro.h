#ifndef  HT_MACRO_H_
#define  HT_MACRO_H_

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>


// TODO: create item needs to know what is he creating

#define FNV_offset 0x811c9dc5
#define FNV_prime_1 713724233 
#define FNV_prime_1a 8473966301
#define LOAD_FACTOR 0.7
#define INIT_SIZE   32


typedef unsigned int uint;



static uint hash_func(const char* key,uint times,uint size);
static uint fnv_1(const char* key);
static uint fnv_1a(const char* key);

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


#define mk_ht(type,NAME_TOKEN) \
    \
    typedef struct ht_item_##NAME_TOKEN{\
        char* key;\
        type value;\
    } ht_item_##NAME_TOKEN;\
    typedef  struct ht_table_##NAME_TOKEN{\
        uint size;\
        uint count;\
        ht_item_##NAME_TOKEN** items;\
    } ht_table_##NAME_TOKEN;\
    \
    static ht_item_##NAME_TOKEN DEAD_ITEM_##NAME_TOKEN; /*= (ht_item_##NAME_TOKEN*){.key=0,.value=0};*/\
    static ht_item_##NAME_TOKEN* ht_create_##NAME_TOKEN(const char* key,const type value);\
    static void ht_free_##NAME_TOKEN(ht_item_##NAME_TOKEN* item);\
    ht_table_##NAME_TOKEN* ht_create_table_##NAME_TOKEN();\
    void ht_free_table_##NAME_TOKEN(ht_table_##NAME_TOKEN* table);\
    void ht_insert_##NAME_TOKEN(ht_table_##NAME_TOKEN* table,const char* key, const type value);\
    type ht_get_##NAME_TOKEN(ht_table_##NAME_TOKEN* table,const char* key);\
    void ht_resize_##NAME_TOKEN(ht_table_##NAME_TOKEN* table);\
    \
    static ht_item_##NAME_TOKEN* ht_create_item_##NAME_TOKEN(const char* key,const type value) {\
        ht_item_##NAME_TOKEN* item = (ht_item_##NAME_TOKEN*) malloc(sizeof(ht_item_##NAME_TOKEN));\
        item->key   = strdup(key);\
        item->value = value;\
        return item;\
    }\
    static void ht_free_item_##NAME_TOKEN(ht_item_##NAME_TOKEN* item) {\
        if(!item || item == &DEAD_ITEM_##NAME_TOKEN) return; \
        free((void*)item->key);\
        free(item);\
    }\
    ht_table_##NAME_TOKEN* ht_create_sized_table_##NAME_TOKEN(uint size) {\
        assert(size > 0 && "size must be a positive integer");\
        ht_table_##NAME_TOKEN* table= (ht_table_##NAME_TOKEN*) malloc(sizeof(ht_table_##NAME_TOKEN));\
        table->count = 0;\
        table->size = size;\
        table->items = (ht_item_##NAME_TOKEN**) malloc(sizeof(ht_item_##NAME_TOKEN*) * size);\
        for (size_t i = 0; i < size; i++) {\
            table->items[i] = NULL;\
        }\
        return table;\
    }\
    ht_table_##NAME_TOKEN* ht_create_table_##NAME_TOKEN() { \
        return ht_create_sized_table_##NAME_TOKEN(INIT_SIZE);\
    }\
    void ht_free_table_##NAME_TOKEN(ht_table_##NAME_TOKEN* table) {\
        for (size_t i = 0; i < table->size; i++) {\
            ht_free_item_##NAME_TOKEN(table->items[i]);\
        }\
        free(table->items);\
        table->size  = 0;\
        table->count = 0;\
        free(table);\
        table = NULL;\
    }\
    void ht_insert_##NAME_TOKEN(ht_table_##NAME_TOKEN* table,const char* key,const type value) {\
        uint times = 0;\
        uint i = 0;\
        if((float)table->count/table->size  >= LOAD_FACTOR) {\
            ht_resize_##NAME_TOKEN(table);\
        }\
        ht_item_##NAME_TOKEN** dead_item = NULL;\
        do {\
            i = hash_func(key,times,table->size);\
            times++;\
            if(table->items[i] == &DEAD_ITEM_##NAME_TOKEN){\
                dead_item = &table->items[i];\
            }\
            if(table->items[i] != NULL &&  table->items[i] != &DEAD_ITEM_##NAME_TOKEN && strcmp(table->items[i]->key,key) == 0) {\
                table->items[i]->value = value;\
                return;\
            }\
        } while(table->items[i] != NULL);\
        ht_item_##NAME_TOKEN* item = ht_create_item_##NAME_TOKEN(key,value);\
        if(dead_item != NULL) {\
            *dead_item = item;\
        } else {\
            table->items[i] = item;\
        }\
        table->count++;\
    }\
    type ht_get_##NAME_TOKEN(ht_table_##NAME_TOKEN* table,const char* key) {\
        uint times = 0;\
        uint i = 0; \
        do {\
            i = hash_func(key,times,table->size);\
            times++;\
            if(table->items[i] == NULL) {\
                return 0; \
            }\
        } while(table->items[i] == &DEAD_ITEM_##NAME_TOKEN || strcmp(table->items[i]->key,key) != 0);\
        return table->items[i]->value;\
    }\
    void ht_remove(ht_table_##NAME_TOKEN* table,const char* key) { \
        uint times = 0;\
        uint i = 0; \
        do {\
            i = hash_func(key,times,table->size);\
            times++;\
            if(table->items[i] == NULL) {\
                return;\
            }\
        } while(strcmp(table->items[i]->key,key) != 0);\
        ht_free_item_##NAME_TOKEN(table->items[i]);\
        table->items[i] = &DEAD_ITEM_##NAME_TOKEN;\
        table->count++;\
    }\
    void ht_resize_##NAME_TOKEN(ht_table_##NAME_TOKEN* table) {\
        uint i = 0;\
        uint counted = 0;\
        ht_table_##NAME_TOKEN* t = ht_create_sized_table_##NAME_TOKEN(table->size * 2);\
        while(i < table->size && counted < table->count ) {\
            if(table->items[i] != NULL && table->items[i] != &DEAD_ITEM_##NAME_TOKEN) {\
                ht_insert_##NAME_TOKEN(t,table->items[i]->key,table->items[i]->value);\
                counted++;\
            }\
            i++;\
        }\
        ht_item_##NAME_TOKEN** tmp_items = table->items;\
        table->items = t->items;\
        table->size= t->size;\
        t->items = tmp_items;\
        t->size= t->size / 2;\
        ht_free_table_##NAME_TOKEN(t);\
    }



mk_ht(int,int);

#undef FNV_offset   
#undef FNV_prime_1  
#undef FNV_prime_1a 
#undef LOAD_FACTOR 
#undef INIT_SIZE    
#endif
