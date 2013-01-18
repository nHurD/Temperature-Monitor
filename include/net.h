//
//  net.h
//  BeagleBone
//
//  Created by Jonathan Steele on 1/14/13.
//
//

#ifndef BeagleBone_net_h
#define BeagleBone_net_h

void handle_tcp_connection ( int ) ;
int create_tcp_socket ( unsigned short );
int accept_tcp_connection ( int );

#endif
