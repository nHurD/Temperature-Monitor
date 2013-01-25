//
//  sqlite.h
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/13/13.
//
//

#ifndef _sqlite_h
#define _sqlite_h

#include <time.h>

#define DB_FILE "temperature_data.db"

typedef struct {
    char *sensor_id;
    float temperature;
    time_t time_information;
} tempdata_t;


int init_connection ( char * );
int close_connection ( );
int create_database ( char * );
int insert_data ( tempdata_t * );

void *sqlite_thread ( void * );


#endif
