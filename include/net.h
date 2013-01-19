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


#define DIE(msg) { perror ( msg ); exit ( 1 ); }

int handle_tcp_connection ( int, char*(*)(int, char[RECV_BUF_SIZE]) );
int create_tcp_socket ( unsigned short );
int accept_tcp_connection ( int );

void run_server ( char*(*)( int, char[RECV_BUF_SIZE]) );

#endif
