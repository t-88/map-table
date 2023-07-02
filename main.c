#include <stdio.h>
#include "ht.h"


int main(void) {
    ht_table* table = ht_create_table();
    ht_insert(table,"key1","item1");
    ht_insert(table,"key2","item2");
    ht_insert(table,"key2","item2");
    ht_insert(table,"key3","item3");
    
    ht_print_table(table);
    printf("val: %s\n",ht_get(table,"key2")); 

    ht_remove(table,"key1");
    ht_print_table(table);



    ht_insert(table,"key3","item4");
    ht_print_table(table);


    
    ht_insert(table,"key1","1");
    ht_print_table(table);
    

    for(int i = 0; i < 128; i++) {
        char s[4];
        sprintf(s,"%d",i);
        ht_insert(table,s,s);
    } 
    

    ht_print_table(table);

    ht_free_table(table);
    return 0;
} 
