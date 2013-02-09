//
//  monitor.h
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 2/9/13.
//
//

#ifndef _monitor_h
#define _monitor_h

void *monitor_thread ( );

void *create_shared_segment ( size_t, int );

void detach_memory_segment ( void ** );

void *get_shared_segment ( size_t );


#endif
