//
//  main.c
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/11/13.
//
//

#include <stdio.h>

#include "sqlite.h"
#include "w1.h"
#include "net.h"
#include "callbacks.h"
#include "daemon.h"




int main ( int argc, char **argv ) {
    
   // run_server ( net_communication_callback );
    
    daemonize ( );
    run_as_daemon ( argc, argv );
    
    return 0;
}




