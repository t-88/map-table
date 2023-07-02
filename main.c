#include <stdio.h>
#include "ht_macro.h"


#define NUM_OF_TEST_VALUES 1000



int main(void) {
    ht_table_int* table = ht_create_table_int();
   	for (int i=0; i<NUM_OF_TEST_VALUES; ++i) {
		char buff[4]; 
        sprintf(buff, "%d", i);
		ht_insert_int(table,buff , i);
    }

#if 0
    ht_print_table(table);
    printf("%d\n",table->count);
    
#endif
    printf("%d",ht_get_int(table,"4"));
    ht_free_table_int(table);
    return 0;
} 
