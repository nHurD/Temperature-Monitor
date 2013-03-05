//
//  daemon.h
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/22/13.
//
//

#ifndef _daemon_h
#define _daemon_h


#define LOG_FILE "daemon.log"

typedef struct {
    int poll_interval;
    char *temperature_scale;
    int listen_port;
    char *database_file;
    char *run_dir;
} settings_t;


void sig_handler ( int );

void daemonize ( char *, char * );

char *get_lock_file ( );

void run_as_daemon ( int, char ** );


/* Settings */
settings_t *read_config_file ( char * );

#define CFG_GROUP_NAME "global"
#define CFG_POLL_INTERVAL "poll_interval"
#define CFG_TEMP_SCALE "scale"
#define CFG_LISTEN_PORT "port"
#define CFG_WORKING_DIR "working_dir"
#define CFG_DATABASE_FILE "database_file"



#endif
