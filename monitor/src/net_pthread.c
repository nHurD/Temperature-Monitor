//
//  net_pthread.c
//  TemperatureMonitor
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
#include "w1.h"
#include "callbacks.h"

void *handle_tcp_connection ( void *ptr ) {
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
                    while ( message_finished == 0  && no_error == 1 ) {
                        read_len = recv ( arg->socket, buffer, RECV_BUF_SIZE, 0 );
                        
                        /* Recv error */
                        if ( read_len < 0 ) {
                            perror ( "recv()" );
                            no_error = 0;
                            break;
                        }
                        
                        /* Client closed connection */
                        if ( read_len == 0 ) {
                            FD_CLR ( arg->socket, &readSet );
                            close ( arg->socket );
                            no_error = 0;
                            break;
                        }
                        
                        if ( message == NULL ) {
                            message = malloc ( RECV_BUF_SIZE + 1 );
                            
                            strncpy ( message, buffer, RECV_BUF_SIZE );
                        } else {
                            /* Resize as necessary */
                            char *tmp;
                            //tmp = realloc ( message, strlen ( message ) + read_len * sizeof ( char ) + 1 );
                            
                            tmp = malloc ( strlen ( message ) + read_len + 1 );
                            
                            
                            /* Sanity check */
                            if (tmp == NULL ) {
                                DIE ( "realloc()." );
                            }
                            
                            memset ( tmp, '\0', strlen ( message ) + read_len + 1 );
                            
                            strncpy ( tmp, message, strlen ( message ) );
                            
                            strncat ( tmp, buffer, RECV_BUF_SIZE );
                            
                            message = tmp;
                            
                          
                        }
                        
                        if ( strstr ( message, "\n") != NULL ) {
                            message_finished = 1;
                            read_len = strcspn ( message, "\n" );
                            
                            message [read_len +1 ] = '\0';
                            
                           
                        }
                    }
                    
                   
                    
                    
                    if ( arg->call_back != NULL && no_error == 1 ) {
                        
                        char *data = arg->call_back ( message );
                        
                        free ( message );
                        
                        message = NULL;
                        
                        if ( send ( arg->socket, data, strlen ( data ), 0 ) != strlen ( data ) ) {
                            no_error = 0;
                            perror ( "Error sending data" );
                        }
                        
                        free ( data );
                        
                        data = NULL;
                        
                        
                    }
                        
                    
                }
        }
    }
    
    free ( arg->attr );
    free ( arg );
    
    return NULL;
}


void run_server ( char* ( *callback ) ( char* ) ) {
    int listener;
    arg_t *arg;
    
    pthread_t thread = (pthread_t)NULL;
    
    listener = create_tcp_socket ( LISTEN_PORT );
    
    
    for ( ;; ) {
        arg = malloc ( sizeof (arg_t ) );
        
        if (arg == NULL ) {
            DIE ("malloc() arg." );
            
        }
        
        arg->call_back = callback;
        arg->socket = accept_tcp_connection ( listener );
        arg->attr = malloc ( sizeof ( pthread_attr_t ) );
        
        pthread_attr_init ( arg->attr );
        pthread_attr_setdetachstate ( arg->attr, PTHREAD_CREATE_DETACHED );
        pthread_create( &thread, arg->attr, handle_tcp_connection, arg );
        
        
        
    }
    
    
    
}

#endif