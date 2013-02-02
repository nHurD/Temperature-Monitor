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


void sig_handler ( int );

void daemonize ( char *, char * );

void run_as_daemon ( int, char ** );


#endif
