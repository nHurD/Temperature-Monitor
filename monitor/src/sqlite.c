//
//  sqlite.c
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/13/13.
//
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>

#include "sqlite.h"

#define SANITY_CHECK(result, expected, error)  {                           \
                        if (result != expected) {                          \
                            printf ( "%s, code: %d\n\tMessage: %s\n",      \
                                      error,                               \
                                      result,                              \
                                      sqlite3_errmsg ( _ctx )              \
                            );                                             \
                            return result;                                 \
                        }                                                  \
        }




#define CREATE_STATEMENT "CREATE TABLE TemperatureData (                \
                            row_id INTEGER PRIMARY KEY AUTOINCREMENT,   \
                            time_data   DATETIME,                       \
                            sensor_id   TEXT,                           \
                            temperature REAL                            \
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


/* FIXME: Change function calls to eliminate global variable */
sqlite3 *_ctx;

/* Init Connection:
 *  Open the specified Sqlite Database.
 *  If no database is found at the given path, make an attempt at creating it first.
 */
int init_connection ( char *path ) {
    int result = 0;
    
    /* Try to open the specified database */
    result = sqlite3_open_v2 ( path, &_ctx, SQLITE_OPEN_READWRITE | SQLITE_OPEN_NOMUTEX, NULL );
    
    /* Sanity check */
    if ( result != SQLITE_OK ) {
        sqlite3_close( _ctx );
        
        printf ( "Error opening: %d\n", result );
        
        /* Try to create the database */
        result = sqlite3_open ( path, &_ctx );
        
        
        result = create_database ( path );
        
        return result;
        
    }
    
    return result;

}

/* Close Connections:
 *  Close the current connection to the database
 */
int close_connection ( ) {
    return sqlite3_close ( _ctx );
}

/* Create Database:
 *  Create a database at the specified path
 */
int create_database ( char *path ) {
    int             result = 0;
    sqlite3_stmt    *statement;
    
    
    /* If the connection is null, initialize the connection before proceeding */
    if ( _ctx == NULL ) {
        result = init_connection ( path );
        
        SANITY_CHECK ( result, SQLITE_OK, "init" );
    
    }
    
    /* Prepare the Create table statement for execution, and ensure it's ready to go */
    result = sqlite3_prepare ( _ctx,
                               CREATE_STATEMENT,
                               strlen(CREATE_STATEMENT),
                               &statement,
                               NULL
                             );
                              
    SANITY_CHECK ( result, SQLITE_OK, "prepare create" );
    
    /* Perform the execution, and ensure we got our expected results */
    result = sqlite3_step ( statement );
    
    SANITY_CHECK ( result, SQLITE_DONE, "step create" );
    
    sqlite3_finalize ( statement );
    
    return result;
}

/* Insert Data:
 *  Insert data into the Temperature Data table for later analysis
 */
int insert_data ( tempdata_t *data ) {
    int             result = 0;
    sqlite3_stmt    *statement;
    
    /* Prepare our statement */
    result = sqlite3_prepare ( _ctx,
                               INSERT_STATEMENT,
                               strlen( INSERT_STATEMENT ),
                               &statement,
                               NULL
                            );
    
    SANITY_CHECK ( result, SQLITE_OK, "prepare insert" );
    
    /* Convert time_t to char * */
    char buffer[40];
    strftime ( buffer, 40, "%Y-%m-%d %H:%M:%S", localtime ( &data->time_information ) );
    
    /* Bind the time data to the statement */
    result = sqlite3_bind_text ( statement,                     /* Time Data */
                                 1,
                                 buffer,
                                 -1,
                                 SQLITE_TRANSIENT
                                );
    
    SANITY_CHECK ( result, SQLITE_OK, "bind time" );
    
    /* Bind the sensor id to the statement */
    result = sqlite3_bind_text ( statement,                     /* Sensor ID */
                                 2,
                                 data->sensor_id,
                                 strlen( data->sensor_id ),
                                 SQLITE_STATIC
                               );
    
    SANITY_CHECK ( result, SQLITE_OK, "bind sensor" );


    /* Bind the temperature data to the statement */
    result = sqlite3_bind_double ( statement,                   /* Temperature */
                                  3,
                                  data->temperature
                                 );
    
    SANITY_CHECK ( result, SQLITE_OK, "bind temperature" );
    
    /* Execute and perform a sanity check */
    result = sqlite3_step ( statement );
    
    SANITY_CHECK ( result, SQLITE_DONE, "step insert" );
    
    sqlite3_finalize ( statement );
    
    return result;
}



