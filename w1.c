//
//  w1.c
//  BeagleBone
//
//  Created by Jonathan Steele on 1/9/13.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "w1.h"

#define PARSE_PATH(path, value, buffer) {               \
            int len = snprintf(NULL, 0, path, value);   \
            buffer = malloc((len+1) * sizeof(char));    \
            len = snprintf(buffer,len+1,path,value);    \
}


#define TO_FARENHEIT(C) (1.8 * C) + 32

#define BUFF_SIZE 16

/* List all of the slaves on a given w1 bus */
extern int list_slaves ( int busid, char **result ) {
    
    int counter     = 0;
    char *path      = NULL;
    int  fd         = 0;
    
    char buf[ROM_CODE_LENGTH];
    
  
    /* Initialize the 2D array */
    result[counter] = malloc ( ROM_CODE_LENGTH * sizeof *result[0] );
    
    
    /* Parse the path for our w1 bus */
    PARSE_PATH ( W1_PATHS[0], busid, path );
    
    /* Try to open the bus */
    fd = open ( path, O_RDONLY );
    if ( fd < 0 ) {
        perror ( "list_slaves/open" );
        exit ( 1 );
    }
    
    /* Read information from our bus. */
    while ( read ( fd, buf, ROM_CODE_LENGTH ) > 0 ) {
       
        /* Null terminate the last item in the buffer */
        buf[(ROM_CODE_LENGTH-1)] = '\0';
        
        /*
         * Because this is actually not a file, but a device, make sure we're not repeating ourselves.
         * If so, break out of the loop, and clean up
         */
        if ( buf == result[0] ) { break; }
        
        
        /* Resize our array to store the new information */
        if ( counter > 0 ) {
            result = realloc ( result, sizeof(**result) + sizeof *result );
            result[counter] = malloc ( ROM_CODE_LENGTH * sizeof *result[counter] );
        }
        
        /* Finally, copy the new data into that string */
        strncpy ( result[counter], buf, sizeof buf );
      
        
        counter++;
    }
    
    /* Clean up */
    close ( fd );
    free ( path );
    
    /* 
     * Sanity check: Make sure we actually found something.
     * If not, set the first item in result to \0 and reset the counter to 0.
     */
    if (strstr(result[0],"not found.") != NULL){
        memset(result[0],'\0',ROM_CODE_LENGTH);
        counter = 0;
    }
    
    
    return counter;
    
}


extern char *read_data ( char *slave ) {
    
    char    *result;
    FILE    *fp;
    char    *file;
    size_t fileLen;
    
    /* Parse the path for our slave device */
    PARSE_PATH ( W1_PATHS[1], slave, file );
    
    fp = fopen ( file, "r");
    
    /* Sanity check for the file */
    if (fp == NULL){
        perror("Unable to open device");
        exit (-1);
    }
    
    /* Determine the file size, then reset to the beginning of the file */
    fseek ( fp, 0, SEEK_END );
    fileLen = ftell ( fp );
    rewind ( fp );
    
    /* Set the size of our result to the size of the file plus padding for \0 */
    result = malloc ( sizeof( char ) * fileLen + 1 );
    result[fileLen] = '\0';
    
    /* Read our data into the string */
    size_t bytesRead = fread ( result, sizeof(char), fileLen, fp );
    
    /* Clean up */
    fclose ( fp );
    
        
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