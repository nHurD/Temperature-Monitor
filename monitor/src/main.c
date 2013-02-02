//
//  main.c
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/11/13.
//
//

#include <stdio.h>
#include <stdlib.h>

#include "sqlite.h"
#include "w1.h"
#include "net.h"
#include "callbacks.h"
#include "daemon.h"




int main ( int argc, char **argv ) {
    
   // run_server ( net_communication_callback );
    
    char *tmp;
   
    int len = snprintf(NULL, 0, "/home/root/%s", argv[0] );
   
    tmp = malloc ( len + 1 );
    
    len = snprintf ( tmp, len+1, "/home/root/%s", argv[0] );
    
    daemonize ( "/home/root/", tmp );
    run_as_daemon ( argc, argv );
    
    return 0;
}




