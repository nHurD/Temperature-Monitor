//
//  w1.h
//  BeagleBone
//
//  Created by Jonathan Steele on 1/9/13.
//
//

#ifndef BeagleBone_w1_h
#define BeagleBone_w1_h

extern int list_slaves ( int busid, char **result );

extern char *read_data ( char *slave );

#endif
