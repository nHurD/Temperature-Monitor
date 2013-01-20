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

#define COMMAND_SIZE 3


float get_temperature_from_data ( char *data );

char *net_communication_callback ( char *data );


int main ( void ) {
    /* Init the array */
   /* char **result = malloc(1 * sizeof *result);
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
    
    free(result); */
    
    run_server ( net_communication_callback );
    
    return 0;
}



char *net_communication_callback ( char * data ) {
    char *result;
    char *cmd;
    char *temporary_value;
    
    char **slaves;

    int str_len = 0;
    int slave_len = 0;
    int i;
    
    cmd = malloc ( COMMAND_SIZE + 1 );
    memset ( cmd, '\0', COMMAND_SIZE + 1 );
    
    strncpy ( cmd, data, COMMAND_SIZE );
    
   
    /* List all slaves on bus 1 */
    if ( strncmp ( "LST", cmd, 3 ) == 0 ) {
        
        slaves = malloc ( 1 * sizeof ( *slaves ) );
        slave_len = list_slaves ( 1, slaves );
        
        if ( slave_len  == 0 ) {
        
            str_len = snprintf ( NULL, 0, "No slaves found.\n" );
            result = malloc ( str_len + 1 );
            str_len = snprintf ( result, str_len + 4, "%d No slaves found.\n", str_len );
        
        } else {
            
            /* Output format: "{slave_id} " */
            result = malloc ( slave_len * ROM_CODE_LENGTH + slave_len + 1 );
            
            memset ( result, '\0', slave_len * ROM_CODE_LENGTH + slave_len + 1 );
            
            for ( i = 0; i < slave_len; i++ ) {
                strcat ( result, slaves[i] );
                strcat ( result, " " );
            }
            
            str_len = snprintf ( NULL, 0, "%d %s", strlen(result), result );
            
            temporary_value = malloc ( str_len );
            
            if ( temporary_value == NULL ) {
                DIE ("malloc() in callback");
            }
            
            str_len  = snprintf ( temporary_value, str_len, "%d %s", strlen(result), result );
            
            result = temporary_value;
            
        }
    }
    
    /* Read Temperature from sensor in Celsius or Farenheit */
    if (   strncmp ( "RTC", cmd, 3 ) == 0
        || strncmp ( "RTF", cmd, 3 ) == 0 ) {
        
        char *tmp = data;
        
        /* Set pointer to start of sensor data */
        tmp += 4;
        
        tmp[ strlen ( tmp ) - 1 ] = 0;
        
        printf ( "Data: '%s'\n", tmp );
        
        float temperature = get_temperature_from_data ( read_data ( tmp ) );
        
        if ( strncmp ( "RTF", cmd, 3 ) == 0 ) {
            temperature = TO_FARENHEIT ( temperature );
        }
        
        str_len = snprintf ( NULL, 0, "%f", temperature );
        
        temporary_value = malloc ( str_len + 1 );
        memset ( temporary_value, '\0', str_len + 1 );
        
        str_len = snprintf ( temporary_value, str_len, "%f", temperature );
        
        str_len = snprintf ( NULL, 0, "%d %f", strlen( temporary_value ), temperature );
        
        result = malloc ( str_len + 1 );
        
        str_len = snprintf( result, str_len, "%d %f", strlen ( temporary_value ), temperature );
        
        free ( temporary_value );
    }
    
    if (result == NULL ) {
        result = malloc ( 20 );
        
        memset ( result, '\0', 20 );
        
        str_len  = snprintf ( result, 19, "16 Unknown command." );
        
    }
    
    
    return result;
    
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

