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

typedef struct {
    char *sensor_id;
    float temperature;
    time_t time_information;
} DATA;

int init_connection ( char * );
int close_connection ( );
int create_database ( char * );
int insert_data ( DATA * );


#endif
