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
    
    return result;
    
}
