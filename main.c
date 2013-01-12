//
//  main.c
//  BeagleBone
//
//  Created by Jonathan Steele on 1/11/13.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "w1.h"

#define TO_FARENHEIT(C) (1.8 * C) + 32


float get_temperature_from_data ( char *data ) ;


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


float get_temperature_from_data ( char *data ) {
    float result = 0.0;
    
    /* Find the location of the start of the temperature informaiton */
    char *pos = strstr ( data, "t=" );
    
    /* Sanity check, if not found, throw error */
    if ( pos == NULL ) {
        perror("No Data");
        exit(1);
    }
    
    /* Move the pointer past t= */
    pos += 2;
    
    /* Set Null char at end */
    pos[strlen(pos)-1] = '\0';
    
    /* Convert to float, then divide by 1000 */
    result = atof(pos) / 1000;
    
    return result;
    
}

