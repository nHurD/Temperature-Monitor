//
//  callbacks.c
//  BeagleBone
//
//  Created by Jonathan Steele on 1/24/13.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "w1.h"
#include "net.h"
#include "callbacks.h"

char *net_communication_callback ( char * data ) {
    char *result = NULL;
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
