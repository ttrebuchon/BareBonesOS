#ifndef INCLUDED_IDE_SYMBOLS_HH
#define INCLUDED_IDE_SYMBOLS_HH

#include <Common.h>

namespace Drivers { namespace IDE {

	
    #define IDE_STATE_BUSY 0x80
	#define IDE_STATE_READY 0x40
	#define IDE_STATE_FAULT 0x20
	#define IDE_STATE_SEEK_DONE 0x10
	#define IDE_STATE_REQUEST_READY 0x08
	#define IDE_STATE_CORRECTED_DATA 0x04
	#define IDE_STATE_INLEX 0x02
	#define IDE_STATE_ERROR 0x01

	#define IDE_PRIMARY 0x00
	#define IDE_SECONDARY 0x01
	
	#define IDE_MASTER 0xA0
	#define IDE_SLAVE 0xB0

	#define IDE_REG_DATA 0x00
	#define IDE_REG_ERROR 0x01
	#define IDE_REG_FEATURES 0x01
	#define IDE_REG_SECTOR_COUNT_0 0x02
	#define IDE_REG_LBA_LOW 0x03
	#define IDE_REG_LBA_MID 0x04
	#define IDE_REG_LBA_HIGH 0x05
	#define IDE_REG_DEVICE_SELECT 0x06
	#define IDE_REG_COMMAND 0x07
	#define IDE_REG_STATUS 0x07
	#define IDE_REG_SECTOR_COUNT_1 0x08
	#define IDE_REG_LBA_3 0x09
	#define IDE_REG_LBA_4 0x0A
	#define IDE_REG_LBA_5 0x0B
	#define IDE_REG_CONTROL 0x0C
	#define IDE_REG_ALT_STATUS 0x0C
	#define IDE_REG_DEVICE_ADDR 0x0D

	
}
}

#endif