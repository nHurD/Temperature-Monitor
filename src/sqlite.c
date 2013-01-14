//
//  sqlite.c
//  BeagleBone
//
//  Created by Jonathan Steele on 1/13/13.
//
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

#include "sqlite.h"

#define SANITY_CHECK(result, expected, error) { if (result != expected) { printf("%s\n", error); return result; } }

#define CREATE_STATEMENT "CREATE TABLE TemperatureData (       \
                            time_data   DATETIME PRIMARY KEY,  \
                            sensor_id   TEXT,                  \
                            temperature REAL                   \
                         );"

#define INSERT_STATEMENT "INSERT INTO TemperatureData (     \
                            time_data,                      \
                            sensor_id,                      \
                            temperature                     \
                          ) VALUES (                        \
                            @time_data,                     \
                            @sensor_id,                     \
                            @temperature                    \
                          );"


sqlite3 *_ctx;

int init_connection ( char *path ) {
    int result = 0;
    
    result = sqlite3_open ( path, &_ctx );
    
    /* Sanity check */
    if ( result != SQLITE_OK ) {
        sqlite3_close( _ctx );
        return result;
        
    }
    
    return result;

}

int close_connection ( ) {
    return sqlite3_close ( _ctx );
}

int create_database ( char *path ) {
    int             result = 0;
    sqlite3_stmt    *statement;
    
    
    if ( _ctx == NULL ) {
        result = init_connection ( path );
        
        SANITY_CHECK ( result, SQLITE_OK, "init" );
    
    }
    
    result = sqlite3_prepare ( _ctx,
                               CREATE_STATEMENT,
                               strlen(CREATE_STATEMENT),
                               &statement,
                               NULL
                             );
                              
    SANITY_CHECK ( result, SQLITE_OK, "prepare create" );
    
    result = sqlite3_step ( statement );
    
    SANITY_CHECK ( result, SQLITE_DONE, "step create" );
    
    sqlite3_finalize ( statement );
    
    return result;
}

int insert_data ( DATA *data ) {
    int             result = 0;
    sqlite3_stmt    *statement;
    
    result = sqlite3_prepare ( _ctx,
                               INSERT_STATEMENT,
                               strlen( INSERT_STATEMENT ),
                               &statement,
                               NULL
                            );
    
    SANITY_CHECK ( result, SQLITE_OK, "prepare insert" );
    
    char buffer[20];
    
    strftime ( buffer, 20, "%Y-%m-%d %H:%M:%S", localtime ( &data->time_information ) );
    
    result = sqlite3_bind_text ( statement,
                                 1,
                                 buffer,
                                 -1,
                                 SQLITE_TRANSIENT
                                );
    
    SANITY_CHECK ( result, SQLITE_OK, "bind time" );
    
    result = sqlite3_bind_text ( statement,                     /* Sensor ID */
                                 2,
                                 data->sensor_id,
                                 strlen( data->sensor_id ),
                                 SQLITE_STATIC
                               );
    
    SANITY_CHECK ( result, SQLITE_OK, "bind sensor" );

    
    result = sqlite3_bind_double ( statement,                   /* Temperature */
                                  3,
                                  data->temperature
                                 );
    
    SANITY_CHECK ( result, SQLITE_OK, "bind temperature" );
    
    result = sqlite3_step ( statement );
    
    SANITY_CHECK ( result, SQLITE_DONE, "step" );
    
    sqlite3_finalize ( statement );
    
    return result;
}




/* int main ( void ) {
    int result = create_database ( "test.db" );
    DATA data;
    
    data.temperature = 61.2;
    data.sensor_id = "test";
    data.time_information = time ( NULL );
    
    result = insert_data ( &data );
    
    printf ( "%d\n", result );
    
    
    
    close_connection ();
    
    return 0;
} */


