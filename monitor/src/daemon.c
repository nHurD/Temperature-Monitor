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
#include <unistd.h>
#include <syslog.h>
#include <glib.h>

#include "daemon.h"
#include "net.h"
#include "callbacks.h"
#include "sqlite.h"


int pid_file_handle;

char *lockFile = NULL;

char *get_lock_file ( ) {
    return lockFile;
}

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
            syslog ( LOG_WARNING, "Received SIGHUP signal." );
            break;
        case SIGTERM:
            syslog ( LOG_INFO, "Shutting down." );
            close_connection ( );
            exit ( EXIT_SUCCESS );
            break;
    }
    
}


void daemonize ( char *run_dir, char *pid_file ) {
    int pid, sid, i;
    char pid_str[10];
    struct sigaction sAction;
    sigset_t newSigSet;
    
    if ( getppid ( ) == 1 ) {
        return;
    }
    
    /* set the value of lockFile */
    int len = snprintf(NULL, 0, "%s/%s", run_dir, pid_file );
    lockFile = malloc ( len + 1 );
    memset ( lockFile, '\0', len + 1 );
    
    len = snprintf(lockFile, len, "%s/%s", run_dir, pid_file );

    
    sigemptyset ( &newSigSet );
    sigaddset ( &newSigSet, SIGCHLD );
    sigaddset ( &newSigSet, SIGTSTP );
    sigaddset ( &newSigSet, SIGTTOU );
    sigaddset ( &newSigSet, SIGTTIN );
    sigprocmask ( SIG_BLOCK, &newSigSet, NULL );
    
    sAction.sa_handler = sig_handler;
    sigemptyset ( &sAction.sa_mask );
    sAction.sa_flags = 0;
    
    sigaction ( SIGHUP, &sAction, NULL );
    sigaction ( SIGTERM, &sAction, NULL );
    sigaction ( SIGINT, &sAction, NULL );
    
    pid = fork ( );
    
    if ( pid < 0 ) {
        exit ( EXIT_FAILURE );
    }
    
    if ( pid > 0 ) {
        printf ("Child process created: %d\n", pid );
        exit ( EXIT_SUCCESS );
    }
    
    /* Child section */
    umask ( 027 );
    
    sid = setsid ( );
    
    if ( sid < 0 ) {
        exit ( EXIT_FAILURE );
    }
    
    for ( i = getdtablesize ( ); i >= 0; --i ) {
        close ( i );
    }
    
    close ( STDIN_FILENO );
    close ( STDOUT_FILENO );
    close ( STDERR_FILENO );
    
    chdir ( run_dir );
    
    pid_file_handle = open ( pid_file, O_RDWR|O_CREAT, 0600 );
    
    if ( pid_file_handle == -1 ) {
        syslog ( LOG_INFO, "Could not open PID lock file %s, exiting", pid_file );
        exit ( EXIT_FAILURE );
        
    }
    
    if ( lockf ( pid_file_handle, F_TLOCK, 0 ) == -1 ) {
        syslog ( LOG_INFO, "Could not lock PID lock file %s, exiting", pid_file );
        exit ( EXIT_FAILURE );
    }
    
    sprintf( pid_str, "%d\n", getpid ( ) );
    
    write ( pid_file_handle, pid_str, strlen ( pid_str ) );
    
    
    
}


void run_as_daemon ( int argc, char **argv ) {

    char *tmp;
    
    /* Create thread for sqlite storage */
    pthread_t sqlite;
    
    settings_t *settings;
    
    pthread_attr_t *atr;
    
    
    settings = read_config_file ( "daemon.cfg" );
    
    int len = snprintf(NULL, 0, "%s.pid", argv[0] );
    
    tmp = malloc ( len + 1 );
    
    len = snprintf ( tmp, len+1, "%s.pid", argv[0] );
    
    daemonize ( settings->run_dir, tmp );

    
    atr = malloc ( sizeof ( pthread_attr_t ) );
    
    pthread_attr_init ( atr );
    
    
    pthread_create ( &sqlite,
                     atr,
                     sqlite_thread,
                     settings
                   );
    
    
    
    run_server ( &settings, net_communication_callback );

}