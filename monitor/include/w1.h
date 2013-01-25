//
//  w1.h
//  TemperatureMonitor
//
//  Created by Jonathan Steele on 1/9/13.
//
//

#ifndef _w1_h
#define _w1_h


#define ROM_CODE_LENGTH 16

/* List Slaves:
 * List all slaves given on a bus (busid) and return the total count, and the results
 */
extern int list_slaves ( int busid, char **result );

/* Read Data:
 *  Read information for a given slave 
 */
extern char *read_data ( char *slave );

#endif
