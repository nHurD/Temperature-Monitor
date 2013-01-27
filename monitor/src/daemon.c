//
//  daemon.c
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/22/13.
//
//

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <pthread.h>

#include "daemon.h"
#include "net.h"
#include "callbacks.h"
#include "sqlite.h"


void log_message ( const char *message, ... ) {
    FILE *logfile;
    
    va_list argptr;

    logfile = fopen ( LOG_FILE, "a" );
    if ( !logfile ) return;
    
    va_start ( argptr, message );
    vfprintf ( logfile, message, argptr );
    
    va_end ( argptr );
    
        
    fclose ( logfile );
    
}


void sig_handler ( int sig ) {
    
    switch ( sig ) {
        case SIGHUP:
            log_message ( "Received SIGHUP\n" );
            break;
        case SIGTERM:
            log_message ( "Received SIGTERM\n" );
            close_connection ( );
            break;
    }
    
}


void daemonize ( ) {
    int i;
    int lock_file;
    char str[10];
    
    /* Already a daemon, return */
    if ( getppid () == 1 ) return;
    
    i = fork ( );
    
    if ( i < 0 ) exit ( 1 ); /* Error in fork() */
    if ( i > 0 ) exit ( 0 ); /* Parent exits */
    
    setuid ( ROOT_UID );
    
    errno = 0;
    
    setsid ( );
    
    /* Close all file descriptors */
    for ( i = getdtablesize ( ); i >= 0; --i ) {
        close ( i );
    }
    
    i = open ( "/dev/null" , O_RDWR );
    dup ( i );
    dup ( i );
    
    umask ( ( mode_t ) 027 );
    
    i = chdir ( RUNNING_DIR );
    
    if ( i < 0 ) {
        DIE ( "Unable to chdir" );
    }
    
    lock_file = open ( LOCK_FILE, O_RDWR | O_CREAT, 0640 );
    
    if ( lock_file < 0 ) exit ( 1 );
    
    if ( lockf ( lock_file, F_TLOCK, 0) < 0 ) exit ( 0 );
    
    
    sprintf (str, "%d\n", getpid ( ) );
    
    log_message ( "Forking into background with pid %d\n", getpid ( ) );
    
    write ( lock_file, str, strlen ( str ) );
    
    /* Ignore the following signals */
    signal ( SIGCHLD, SIG_IGN );
    signal ( SIGTSTP, SIG_IGN );
    signal ( SIGTTOU, SIG_IGN );
    signal ( SIGTTIN, SIG_IGN );
    
    /* Handle these signals */
    signal ( SIGHUP, sig_handler );
    signal ( SIGTERM, sig_handler );
    
    
    
}


void run_as_daemon ( int argc, char **argv ) {
    
    /* Create thread for sqlite storage */
    pthread_t sqlite;
    
    pthread_attr_t *atr;
    
    
    atr = malloc ( sizeof ( pthread_attr_t ) );
    
    pthread_attr_init ( atr );
    
    //pthread_attr_setdetachstate ( atr, PTHREAD_CREATE_DETACHED );
    
    pthread_create ( &sqlite,
                     atr,
                     sqlite_thread,
                     NULL
                   );
    
    
    
    run_server ( net_communication_callback );

}