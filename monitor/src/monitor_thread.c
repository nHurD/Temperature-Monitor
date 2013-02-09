//
//  monitor_thread.c
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/21/13.
//
//

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <syslog.h>


#include "daemon.h"
#include "sqlite.h"
#include "monitor.h"

void *create_shared_segment ( size_t seg_size, int flags ) {
    void *result;
    key_t shared_key;
    int shmid;
    int mode;
    
    if ( ( shared_key = ftok (get_lock_file ( ), 'T' ) ) == -1 ) {
        syslog ( LOG_INFO, "Ftok error." );
        return (void *)(-1);
    }
    
    if ( ( shmid = shmget ( shared_key, seg_size, flags ) ) == -1 ) {
        syslog ( LOG_INFO, "Failed to get shared memory segment." );
        return (void *)(-1);
    }
    
    result = shmat ( shmid, (void *)0, 0 );
    if ( result == (void *)(-1) ) {
        syslog ( LOG_INFO, "Failed to attach to memory segment." );
        return result;
    }
    
    return result;
   
}


void detach_memory_segment ( void **ptr ) {
    if ( shmdt ( *ptr ) == -1 ) {
        syslog ( LOG_INFO, "Unable to detach memory segment." );
    }
}


void *monitor_thread ( ) {
    void *data = create_shared_segment ( sizeof ( tempdata_t **),  0644 | IPC_CREAT );
    
    if ( (int)data == -1 ) {
        goto out;
    }
    
    
detach_out:
    detach_memory_segment ( data );
out:
    return NULL;
}