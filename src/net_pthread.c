//
//  net_pthread.c
//  BeagleBone
//
//  Created by Jonathan Steele on 1/23/13.
//
//

#if defined D_PTHREAD

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#include "net.h"

int handle_tcp_connection ( void *ptr ) {
    char buffer[RECV_BUF_SIZE];
    fd_set readSet;
    
    register arg_t *arg = (arg_t *)ptr;
    register int read_len = 0;
    
    int message_finished;
    int no_error = 1;
    
    char *message = NULL;
    
    while ( no_error == 1 ) {
        
        message_finished  = 0;
        
        FD_ZERO ( &readSet );
        FD_SET ( arg->socket, &readSet );
        
        switch ( select ( arg->socket+1, &readSet, NULL, NULL, NULL ) ) {
            case -1:
                //DIE ("select in thread." );
                perror ( "Select in thread." );
                no_error = 0;
                break;
                
            case 0:
                continue;
            case 1:
                
                if ( FD_ISSET ( arg->socket, &readSet ) ) {
                    while ( message_finished == 0 ) {
                        read_len = recv ( arg->socket, buffer, RECV_BUF_SIZE, 0 );
                        
                        /* Recv error */
                        if ( read_len < 0 ) {
                            perror ( "recv()" );
                            no_error = 0;
                            break;
                        }
                        
                        /* Client closed connection */
                        if ( read_len == 0 ) {
                            close ( arg->socket );
                            no_error = 0;
                            break;
                        }
                        
                        if ( message == NULL ) {
                            message = malloc ( RECV_BUF_SIZE +1 );
                            memset ( message, '\0', RECV_BUF_SIZE + 1 );
                            strncpy ( message, buffer, RECV_BUF_SIZE );
                        } else {
                            /* Resize as necessary */
                            char *tmp;
                            tmp = realloc ( message, strlen ( message ) + read_len * sizeof ( char ) + 1 );
                            
                            /* Sanity check */
                            if (tmp == NULL ) {
                                DIE ( "realloc()." );
                            }
                            
                            message = tmp;
                            /* Append to the message */
                            strncat ( message, buffer, read_len );
                        }
                        
                        if ( strstr ( message, "\n") != NULL ) {
                            message_finished = 1;
                        }
                    }
                    
                    if ( arg->call_back != NULL ) {
                        char *data = arg->call_back ( message );
                        
                        free ( message );
                        
                        if ( send ( arg->socket, data, strlen ( data ), 0 ) != strlen ( data ) ) {
                            no_error = 0;
                            perror ( "Error sending data" );
                        }
                        
                        free ( data );
                        
                        
                    }
                        
                    
                }
        }
    }
    
    free ( arg->attr );
    free ( arg );
    
    return 0;
}


void run_server ( char* ( *callback ) ( char* ) ) {
    
    
}


/* Used for testing */
int main ( void ) {
    
    
    
    return 0;
}


#endif