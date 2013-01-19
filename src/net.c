//
//  net.c
//  BeagleBone
//
//  Created by Jonathan Steele on 1/14/13.
//
//

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "net.h"

#define MAX_PENDING 5


#define LISTEN_PORT 9099

/* Handle tcp connection:
 *  Handle any request coming from a connected client. Once data has been received, send it to the specified handler for processing.
 */
int handle_tcp_connection ( int client_socket, char*(*call_back)(char*) ) {
    char buffer[RECV_BUF_SIZE];
    
    char *message = NULL;
    char *data;
    
    int message_size;
    int read_finished = 0;
    
    while ( read_finished == 0 ) {
    
        message_size = recv ( client_socket, buffer, RECV_BUF_SIZE, 0 );
    
        /* Sanity check */
        if ( message_size < 0 ) {
            DIE ( "recv() failed." );
        }
        
        /* Client disconnected, return that status */
        if ( message_size == 0 ) {
            return 0;
        }
        
        if ( message == NULL ) {
            message = malloc ( message_size * sizeof ( char ) + 1 );
            strncpy ( message, buffer, message_size );
        } else {
            char *tmp;
            tmp = realloc ( message, strlen ( message ) + message_size * sizeof ( char ) + 1 );
            if (tmp == NULL ) {
                DIE ("realloc().");
            }
            message = tmp;
            strncat ( message, buffer, message_size );
        }
        
        if ( strstr ( message, "\n") != NULL ) {
            read_finished = 1;
        }
    }
    
    
    data = call_back( message );
    
    free ( message );
    
    if ( send ( client_socket, data, strlen ( data ), 0 ) != strlen ( data ) ) {
        DIE ( "send() failed." );
    }

    free ( data );
    
    
    /* Still active */
    return 1;
    
}

int create_tcp_socket ( unsigned short port ) {
    int sock;
    struct sockaddr_in addr;
    int yes = 1;
    
    if ( (sock = socket ( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 ) {
        DIE ( "socket() failed." );
    }
    
    if ( ( setsockopt ( sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof ( int ) ) ) < 0 ) {
        DIE ( "setsockopt() failed." );
    }
    
    memset ( &addr, 0, sizeof ( addr ) );
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl ( INADDR_ANY );
    addr.sin_port = htons ( port );
    
    if ( bind ( sock, ( struct sockaddr* ) &addr, sizeof ( addr ) ) < 0 ) {
        DIE ( "bind() failed." );
    }
    
    if ( listen ( sock, MAX_PENDING ) < 0 ) {
        DIE ( "listen() failed." );
    }
    
    return sock;
}

int accept_tcp_connection ( int sock ) {
    int client_socket;
    struct sockaddr_in client_addr;
    unsigned int clientLen;
    
    clientLen = sizeof ( client_addr );
    
    if ( ( client_socket = accept ( sock, ( struct sockaddr * ) &client_addr, &clientLen ) ) < 0 ) {
        DIE ( "accept() failed." );
    }
    
    return client_socket;
    
}

void run_server ( char*(*callback)(char*) ) {
    fd_set master;
    fd_set readSet;
    
    
    int listener;
    int new_fd;
    int max_fd;    
  
    int i;
    
    listener = create_tcp_socket ( LISTEN_PORT );
    
    FD_ZERO ( &master );
    
    FD_SET ( listener, &master );
    
    max_fd = listener;
    
    for ( ;; ) {
        
        readSet = master;
        if ( select ( max_fd + 1, &readSet, NULL, NULL, NULL ) == -1 ) {
            DIE ( "select() failed" );
            
        }
        
        for ( i = 0; i <= max_fd; i++ ) {
            if (FD_ISSET ( i, &readSet ) ) {
                if ( i == listener ) {
                
                    new_fd = accept_tcp_connection ( listener );
                    
                    
                    FD_SET ( new_fd, &master );
                
                    if ( new_fd > max_fd ) {
                        max_fd = new_fd;
                    }
                    
                } else {
                    
                    if ( handle_tcp_connection ( i, callback ) == 0 ) {
                        close ( i );
                        FD_CLR ( i, &master );
                    }
                    
                }
            }
        }
    }
    
    close ( listener );

    
}
