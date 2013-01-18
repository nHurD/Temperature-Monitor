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

#define MAX_PENDING 5
#define RECV_BUF_SIZE 32

#define LISTEN_PORT 9099
#define TIMEOUT 10


#define DIE(msg) { perror ( msg ); exit ( 1 ); }


void handle_tcp_connection ( int client_socket ) {
    char buffer[RECV_BUF_SIZE];
    int message_size;
    
    if ( ( message_size = recv ( client_socket, buffer, RECV_BUF_SIZE, 0 ) ) < 0 ) {
        DIE ( "recv() failed." );
    }
    
    while ( message_size > 0 ) {
        if ( send ( client_socket, buffer, message_size, 0 ) != message_size ) {
            DIE ( "send() failed." );
        }
        
        if ( ( message_size = recv ( client_socket, buffer, RECV_BUF_SIZE, 0 ) ) < 0 ) {
            DIE ( "recv() failed." );
        }
    }
    
    close ( client_socket );
}

int create_tcp_socket ( unsigned short port ) {
    int sock;
    struct sockaddr_in addr;
    
    if ( (sock = socket ( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 ) {
        DIE ( "socket() failed." );
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
    
    printf ( "Handling client %s\n" , inet_ntoa ( client_addr.sin_addr ) );
    
    return client_socket;
    
}


void run_server ( ) {
    int server_socket;
    
    fd_set sock_set;

    
    struct timeval selTimeout;
    int running = 1;
    int descriptor;
    
    server_socket = create_tcp_socket ( LISTEN_PORT );
    
    while (running) {
        FD_ZERO ( &sock_set );
        
        FD_SET ( STDIN_FILENO, &sock_set );
        
        FD_SET ( server_socket, &sock_set );
        
        descriptor = server_socket;
        
        selTimeout.tv_sec = TIMEOUT;
        selTimeout.tv_usec = 0;
        
        if ( select ( descriptor+1,  &sock_set, NULL, NULL, &selTimeout ) == 0  ) {
            printf ( "Timeout reached\n" );
        } else {
            if ( FD_ISSET ( 0, &sock_set ) ) {
                printf ( "Shutting down.\n" );
                getchar ();
                running = 0;
            }
            
            if (FD_ISSET (server_socket, &sock_set ) ) {
                printf ( "Received request on %d: ", LISTEN_PORT );
                handle_tcp_connection ( accept_tcp_connection ( server_socket ) );
            }
        }
        
    }
    
    close ( server_socket );
    

}


/* Testing */
int main ( void ) {
    
    run_server ( );
    
    return 0;
}