//
//  w1.h
//  BeagleBone
//
//  Created by Jonathan Steele on 1/9/13.
//
//

#ifndef BeagleBone_w1_h
#define BeagleBone_w1_h

#define ROM_CODE_LENGTH 16


const char* W1_PATHS[] = {
    "/sys/devices/w1_bus_master%d/w1_master_slaves",
    "/sys/bus/w1/devices/%s"
};

extern int list_slaves(int busid, char **result);

extern void *read_data(char *slave);

#endif
