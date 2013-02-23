//
//  daemon_options.c
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 2/20/13.
//
//

#include <stdio.h>
#include <glib.h>

#include "daemon.h"
#include "net.h"

/* set_default_values:
 *      Define default configuration values
 */
void set_default_values ( settings_t **settings ) {
    
    if ( (*settings)->poll_interval == 0 )
        (*settings)->poll_interval = 10;
    
    if ( (*settings)->temperature_scale == NULL )
        (*settings)->temperature_scale = "C";
    
    if ( (*settings)->listen_port == 0 )
        (*settings)->listen_port = LISTEN_PORT;
    
    if ( (*settings)->database_file == NULL ) 
        (*settings)->database_file = "temperature_data.db";
}


settings_t *read_config_file ( char *file ) {
    settings_t *result;
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;
    gsize length;
    
    /* Create a new key file and set flags */
    keyfile = g_key_file_new ( );
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
    
    
    /* Make an attempt to load the config file. If it fails to load, return NULL */
    if ( !g_key_file_load_from_file (keyfile, file, flags, &error ) ) {
        g_error ( error->message );
        return NULL;
    }
    
    /* Initialize the settings object */
    result = g_slice_new ( settings_t );
    
    
    
    /* Read from the configuration file and store it into our result */
    result->poll_interval = g_key_file_get_integer ( keyfile,
                                                     CFG_GROUP_NAME,
                                                     CFG_POLL_INTERVAL,
                                                     NULL
                                                   );
    
    result->temperature_scale = g_key_file_get_string ( keyfile,
                                                        CFG_GROUP_NAME,
                                                        CFG_TEMP_SCALE,
                                                        NULL
                                                      );
    
    result->listen_port = g_key_file_get_integer ( keyfile,
                                                    CFG_GROUP_NAME,
                                                    CFG_LISTEN_PORT,
                                                    NULL
                                                 );
    
    result->database_file = g_key_file_get_string ( keyfile,
                                                    CFG_GROUP_NAME,
                                                    CFG_DATABASE_FILE,
                                                    NULL
                                                  );

    set_default_values ( &result );
    
    /* Free up the key file memory */
    g_key_file_free ( keyfile );
    
    return result;
    
}
