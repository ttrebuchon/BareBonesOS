#ifndef INCLUDED_DRIVERS_AHCI_FIS_HH
#define INCLUDED_DRIVERS_AHCI_FIS_HH

#include <Common.h>

namespace Drivers { namespace AHCI {


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





    struct FIS_Reg_D2H
    {
        // DWORD 0
        uint8_t fis_type;
        
        uint8_t port_mult:  4;  // Port Multiplier    
        uint8_t reserved0:  2;  
        uint8_t interupt:   1;  //Interrupt
        uint8_t reserved1:  1;

        uint8_t status;
        uint8_t error;

        // DWORD 1
        uint8_t lba0;
        uint8_t lba1;
        uint8_t lba2;
        uint8_t device;

        // DWORD 2
        uint8_t lba3;
        uint8_t lba4;
        uint8_t lba5;
        uint8_t reserved2;

        // DWORD 3
        uint8_t count_low;
        uint8_t count_high;
        uint8_t reserved3[2];

        // DWORD 4
        uint8_t reserved4[4];
    };

    struct FIS_Data
    {
        // DWORD 0
        uint8_t fis_type;

        uint8_t port_multiplier:    4;
        uint8_t reserved0:          4;

        uint8_t reserved1[2];

        // DWORD 1 ~ N
        uint32_t data[1];
    };

    struct FIS_PIO_Setup
    {
        // DWORD 0
        uint8_t fis_type;

        uint8_t port_multiplier:    4;
        uint8_t reserved0:          1;
        uint8_t direction:          1;
        uint8_t interrupt:          1;
        uint8_t reserved1:          1;

        uint8_t status;
        uint8_t error;

        // DWORD 1
        uint8_t lba0;
        uint8_t lba1;
        uint8_t lba2;
        uint8_t device;

        // DWORD 2
        uint8_t lba3;
        uint8_t lba4;
        uint8_t lba5;
        uint8_t reserved2;

        // DWORD 3
        uint8_t count_low;
        uint8_t count_high;
        uint8_t reserved3;
        uint8_t e_status;               // New value of status register

        // DWORD 4
        uint16_t transfer_count;
        uint8_t reserved4[2];
    };


    struct FIS_DMA_Setup
    {
        // DWORD 0
        uint8_t fis_type;

        uint8_t port_multiplier:    4;
        uint8_t reserved0:          1;
        uint8_t direction:          1;
        uint8_t interrupt:          1;
        uint8_t auto_activate:      1;

        uint8_t reserved1[2];

        // DWORD 1 & 2
        uint64_t buffer_ID;             // Used to identify DMA buffer in
                                        // host memory

        // DWORD 3
        uint32_t reserved2;

        // DWORD 4
        uint32_t buffer_offset;         // Byte offset into buffer. First 2 bits must be 0.

        // DWORD 5
        uint32_t transfer_count;        // Number of bytes to transfer

        // DWORD 6
        uint32_t reserved3;
    };
}
}
#endif