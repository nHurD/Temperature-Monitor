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
    
    
    char *tmp;
   
    int len = snprintf(NULL, 0, "/var/lib/get_temperature/%s", argv[0] );
   
    tmp = malloc ( len + 1 );
    
    len = snprintf ( tmp, len+1, "/var/lib/get_temperature/%s", argv[0] );
    
   // daemonize ( "/var/lib/get_temperature/", tmp );
    run_as_daemon ( argc, argv );
    
    
    
    return 0;
}




