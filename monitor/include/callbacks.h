//
//  callbacks.h
//  BeagleBone
//
//  Created by Jonathan Steele on 1/24/13.
//
//

#ifndef BeagleBone_callbacks_h
#define BeagleBone_callbacks_h


#define TO_FARENHEIT(C) (1.8 * C) + 32
#define COMMAND_SIZE 3

float get_temperature_from_data ( char *data );
char *net_communication_callback ( char *data );


#endif
