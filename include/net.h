//
//  net.h
//  BeagleBone
//
//  Created by Jonathan Steele on 1/14/13.
//
//

#ifndef BeagleBone_net_h
#define BeagleBone_net_h

#define RECV_BUF_SIZE 32
#define NET_COMMAND_SIZE 3


#define DIE(msg) { perror ( msg ); exit ( 1 ); }

int handle_tcp_connection ( int, char*(*)( char* ) );
int create_tcp_socket ( unsigned short );
int accept_tcp_connection ( int );

void run_server ( char*(*)( char* ) );

#endif
