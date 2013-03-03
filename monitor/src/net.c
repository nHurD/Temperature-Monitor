//
//  net.c
//  TemperatureMonitor
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
#include "daemon.h"




#ifndef D_PTHREAD
/* Handle tcp connection:
 *  Handle any request coming from a connected client. Once data has been received, send it to the specified handler for processing.
 */
int handle_tcp_connection ( int client_socket, char*(*call_back)(char*) ) {
    char buffer[RECV_BUF_SIZE];
    
    char *message = NULL;
    char *data;
    
    int message_size;
    int read_finished = 0;
    
    /* Loop until the newline character is read */
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
        
        /* Allocate memory for the message */
        if ( message == NULL ) {
            message = malloc ( message_size * sizeof ( char ) + 1 );
            strncpy ( message, buffer, message_size );
        } else {
            /* Resize as necessary */
            char *tmp;
            tmp = realloc ( message, strlen ( message ) + message_size * sizeof ( char ) + 1 );
            
            /* Sanity check */
            if (tmp == NULL ) {
                DIE ( "realloc()." );
            }
            
            message = tmp;
            /* Append to the message */
            strncat ( message, buffer, message_size );
        }
        
        /* Received end of line, finish processing */
        if ( strstr ( message, "\n") != NULL ) {
            read_finished = 1;
        }
    }
    
    /* Pass the message onto the call back handler for processing */
    data = call_back( message );
    
    /* Clean up */
    free ( message );
    
    /* Sned out the results of processing */
    if ( send ( client_socket, data, strlen ( data ), 0 ) != strlen ( data ) ) {
        DIE ( "send() failed." );
    }

    /* Clean up */
    free ( data );
    
    
    /* Still active */
    return 1;
    
}
#endif

/* Create TCP Socket:
 *  Create a TCP server socket for sending / receiving information across the wire.
 */
int create_tcp_socket ( unsigned short port ) {
    int sock;
    struct sockaddr_in addr;
    int yes = 1;
    
    /* Create the socket and fail if unable to do so */
    if ( (sock = socket ( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 ) {
        DIE ( "socket() failed." );
    }
    
    /* Set up the socket options */
    if ( ( setsockopt ( sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof ( int ) ) ) < 0 ) {
        DIE ( "setsockopt() failed." );
    }
    
    /* Initialize the address structure */
    memset ( &addr, 0, sizeof ( addr ) );
    
    /* Specify address details */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl ( INADDR_ANY );
    addr.sin_port = htons ( port );
    
    /* Bind to the socket */
    if ( bind ( sock, ( struct sockaddr* ) &addr, sizeof ( addr ) ) < 0 ) {
        DIE ( "bind() failed." );
    }
    
    /* I CAN HEAR YOU!!! */
    if ( listen ( sock, MAX_PENDING ) < 0 ) {
        DIE ( "listen() failed." );
    }
    
    return sock;
}

/* Accept TCP Connection 
 *  Accept a new incomming request from a client.
 */
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

#ifndef D_PTHREAD
/* Run Server
 *  Function tat will create a new socket, accept and handle connections from clients
 */
void run_server ( settings_t **settings, char* ( *callback ) ( char* ) ) {
    fd_set master;
    fd_set readSet;
    
    
    int listener;
    int new_fd;
    int max_fd;    
  
    int i;
    
    /* Create our socket for handling connections */
    listener = create_tcp_socket ( LISTEN_PORT );
    
    /* Zero out the master fd_set */
    FD_ZERO ( &master );
    
    /* Add the server socket to the set of sockets */
    FD_SET ( listener, &master );
    
    /* The highest file descriptor (fd) */
    max_fd = listener;
    
    /* Infinite loop */
    for ( ;; ) {
        
        /* Copy the master set into a temporary fd */
        readSet = master;
        
        /* Handle multiple connections via Select */
        if ( select ( max_fd + 1, &readSet, NULL, NULL, NULL ) == -1 ) {
            DIE ( "select() failed" );
            
        }
        
        /* Spin through all of the items in readSet and check for connections or requests */
        for ( i = 0; i <= max_fd; i++ ) {
            
            /* We've got a new one! */
            if (FD_ISSET ( i, &readSet ) ) {
                if ( i == listener ) {
                
                    /* Accept a new client */
                    new_fd = accept_tcp_connection ( listener );
                    
                    
                    /* Add it to the list of sockets */
                    FD_SET ( new_fd, &master );
                
                    /* Specify the new max, if need be */
                    if ( new_fd > max_fd ) {
                        max_fd = new_fd;
                    }
                    
                } else {
                    
                    /* Process information coming from a given socket, and close if necessary */
                    if ( handle_tcp_connection ( i, callback ) == 0 ) {
                        close ( i );
                        FD_CLR ( i, &master );
                    }
                    
                }
            }
        }
    }
    
    /* shut down */
    close ( listener );

    
}
#endif

