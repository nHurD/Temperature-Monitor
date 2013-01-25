//
//  daemon.h
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/22/13.
//
//

#ifndef _daemon_h
#define _daemon_h

#define RUNNING_DIR "/home/root"
#define LOCK_FILE "daemon.lock"

#define LOG_FILE "daemon.log"

#define LINE_WIDTH 132

#define ROOT_UID (uid_t)0

void daemonize ( );

void run_as_daemon ( int, char ** );


#endif
