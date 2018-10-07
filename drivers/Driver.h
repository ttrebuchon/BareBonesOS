#ifndef INCLUDED_DRIVERS_DRIVER_H
#define INCLUDED_DRIVERS_DRIVER_H

#include <Common.h>

START_NS(Drivers)
C_CODE

typedef struct
{
	uint8_t data[16];
}
__attribute__((__packed__))
driver_sig_t;

C_END
END_NS


#endif