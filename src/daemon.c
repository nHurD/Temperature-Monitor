//
//  daemon.c
//  BeagleBone
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


#include "daemon.h"
#include "net.h"

void sig_handler ( int sig ) {
 
    
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
    
    chdir ( RUNNING_DIR );
    
    lock_file = open ( LOCK_FILE, O_RDWR | O_CREAT, 0640 );
    
    if ( lock_file < 0 ) exit ( 1 );
    
    if ( lockf ( lock_file, F_TLOCK, 0) < 0 ) exit ( 0 );
    
    sprintf (str, "%d\n", getpid ( ) );
    
    write ( lock_file, strlen ( str ) );
    
    
    
    
}


void run_as_daemon ( ) {
    
}