//
//  sqlite_thread.c
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/24/13.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sqlite3.h>
#include <string.h>
#include <time.h>

#include "sqlite.h"
#include "w1.h"
#include "callbacks.h"

void *sqlite_thread ( void *arg ) {
    
    int status;
    int slave_len;
    int i;
    
    char **slaves;
    
    tempdata_t *temp_data;
    
    status = init_connection ( DB_FILE );
    
    for ( ;; ) {
        
        
        slaves = malloc ( 1 * sizeof ( *slaves ) );
        slave_len = list_slaves ( 1, slaves );
        
        for ( i = 0; i < slave_len; i++ ) {
                        
            temp_data = malloc ( sizeof ( tempdata_t ) );

            temp_data->time_information = time ( NULL );
            temp_data->sensor_id        = malloc ( strlen ( slaves[i] ) + 1 );
            temp_data->temperature      = get_temperature_from_data ( read_data ( slaves[i] ) );;
            
            memset ( temp_data->sensor_id, '\0', strlen ( slaves[i] ) + 1 );
            
            strncpy ( temp_data->sensor_id,
                      slaves[i],
                      strlen ( slaves[i] )
                    );
            
            insert_data ( temp_data );
                    
            //log_message ( "Reading temperature for sensor %s: %f\n", temp_data->sensor_id, temp_data->temperature );
            
            free ( temp_data->sensor_id );
            free ( temp_data );
            
            temp_data = NULL;
            
            
        }
        
        free ( slaves );
        
        slave_len   = 0;
        slaves      = NULL;
        
                
    }

    close_connection ( );
    return NULL;
}
