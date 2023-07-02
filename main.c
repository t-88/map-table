#include <stdio.h>
#include "ht_macro.h"

int main(void) {
    ht_table_int* ti = ht_create_table_int();
    for(int i = 0; i < 100;i++) {
        char buff[4];
        sprintf(buff,"%d",i);
        ht_insert_int(ti,buff,i);
    }
    for(int i = 0; i < 100;i++) {
        char buff[4];
        sprintf(buff,"%d",i);
        printf("key%d: %d\n",i,ht_get_int(ti,buff));
    }
    ht_free_table_int(ti);
    
    getchar();


    ht_table_double* tf = ht_create_table_double();
    for(int i = 0; i < 100;i++) {
        char buff[4];
        sprintf(buff,"%d",i);
        ht_insert_double(tf,buff,(float)i/10.f);
    }
    for(int i = 0; i < 100;i++) {
        char buff[4];
        sprintf(buff,"%d",i);
        printf("key%d: %f\n",i,ht_get_double(tf,buff));
    }
    ht_free_table_double(tf);


    return 0;
} 
