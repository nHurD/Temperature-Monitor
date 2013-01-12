//
//  main.c
//  BeagleBone
//
//  Created by Jonathan Steele on 1/11/13.
//
//

#include <stdio.h>
#include <stdlib.h>
#include "w1.h"

int main ( void ) {
    /* Init the array */
    char **result = malloc(1 * sizeof *result);
    char *data;
    
    int len = 0;
    
    len = list_slaves(1,result);
    
    while(1) {
        data = read_data(result[0]);
        float temperature = get_temperature_from_data( data );
        
        printf("%f\r",TO_FARENHEIT(temperature));
        fflush(stdout);
        free(data);
    }
    
    free(result);
    
    return 0;
}