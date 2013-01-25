//
//  sqlite.h
//  BeagleBone
//
//  Created by Jonathan Steele on 1/13/13.
//
//

#ifndef BeagleBone_sqlite_h
#define BeagleBone_sqlite_h

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
