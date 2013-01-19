//
//  main.c
//  BeagleBone
//
//  Created by Jonathan Steele on 1/11/13.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#include "sqlite.h"
#include "w1.h"
#include "net.h"

#define TO_FARENHEIT(C) (1.8 * C) + 32


float get_temperature_from_data ( char *data ) ;


int main ( void ) {
    /* Init the array */
    char **result = malloc(1 * sizeof *result);
    char *data;
    
    DATA storage_data;
    int save_result = 0;
    
    int len = 0;
    
    len = list_slaves(1,result);
    
    storage_data.sensor_id = result[0];
    
    
    while(1) {
        
        data = read_data(result[0]);
        
        float temperature = get_temperature_from_data( data );
        
        init_connection ( "temperature_data.db" );
        
        storage_data.time_information = time ( NULL );
        
     
        storage_data.temperature = temperature;
        
        save_result = insert_data ( &storage_data );
        
               
        if ( save_result == 1 ) {
            
            free ( data );
            
            data = malloc ( 20 * sizeof ( char ) );
            strftime ( data, 20, "%Y-%m-%d %H:%M:%S", localtime ( &storage_data.time_information ) );
            
            
            printf( "Error saving the following data\n\t Sensor_ID: %s, Tempearature: %f, Date: %s",
                   storage_data.sensor_id,
                   temperature,
                   data
                 );
        }
        
        close_connection ( );
        
        
        printf("%3.2f\r",TO_FARENHEIT(temperature));
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

