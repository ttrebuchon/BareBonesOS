#ifndef INCLUDED_DRIVERS_AHCI_HH
#define INCLUDED_DRIVERS_AHCI_HH

#include <Common.h>

namespace Drivers { namespace Disk {


    enum class FIS_Type
    {
        FIS_Reg_H2D	    = 0x27,	// Register FIS - host to device
        FIS_Reg_D2H	    = 0x34,	// Register FIS - device to host
        FIS_DMA_ACT 	= 0x39,	// DMA activate FIS - device to host
        FIS_DMA_Setup	= 0x41,	// DMA setup FIS - bidirectional
        FIS_DATA		= 0x46,	// Data FIS - bidirectional
        FIS_BIST		= 0x58,	// BIST activate FIS - bidirectional
        FIS_PIO_Setup	= 0x5F,	// PIO setup FIS - device to host
        FIS_Dev_Bits	= 0xA1,	// Set device bits FIS - device to host
    };

    struct FIS_Reg_H2D
    {
        uint8_t fis_type;

        uint8_t pmport  : 4;
        uint8_t rsv0    : 3;
        uint8_t c       : 1;

        uint8_t command;
        uint8_t featurel;

        uint8_t lba0;
        uint8_t lba1;
        uint8_t lba2;
        uint8_t device;

        uint8_t lba3;
        uint8_t lba4;
        uint8_t lba5;
        uint8_t featureh;

        uint8_t countl;
        uint8_t counth;
        uint8_t icc;
        uint8_t control;

        uint8_t rsv1[4];  
    };


}
}
#endif