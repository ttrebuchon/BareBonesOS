#include "IDE.hh"
#include <drivers/PortIO.h>
#include <drivers/PCI.hh>
#include "IDE_Symbols.h"
#include <kernel/Timer.h>
#include <drivers/ATA/ATA_Symbols.hh>
#include <drivers/ATA/ATA.hh>
#include <drivers/ATA/ATAPI.hh>

namespace Drivers { namespace IDE
{
    ChannelRegister_t Channels[2];
    IDEDevice* IDEDevice::Devices[4] = { nullptr, nullptr, nullptr, nullptr };
    bool IDEDevice::__initted = false;



    unsigned char ChannelRegister_t::read(const uint16_t reg)
    {
        if (reg > 0x07 && reg < 0x0C)
        {
            write(IDE_REG_CONTROL, 0x80 | this->nIEN);
        }

        unsigned char result;

        if (reg < 0x08)
        {
            result = port_byte_in(base + reg - 0x00);
        }
        else if (reg < 0x0C)
        {
            result = port_byte_in(base + reg - 0x06);
        }
        else if (reg < 0x0E)
        {
            result = port_byte_in(ctrl + reg - 0x0A);
        }
        else if (reg < 0x16)
        {
            result = port_byte_in(bmide + reg - 0x0E);
        }
        else
        {
            assert(false);
            __builtin_unreachable();
        }

        if (reg > 0x07 && reg < 0x0C)
        {
            write(IDE_REG_CONTROL, this->nIEN);
        }

        return result;
    }

	void ChannelRegister_t::write(const uint16_t reg, unsigned char data)
    {
        if (reg > 0x07 && reg < 0x0C)
        {
            write(IDE_REG_CONTROL, 0x80 | this->nIEN);
        }

        if (reg < 0x08)
        {
            port_byte_out(base + reg - 0x00, data);
        }
        else if (reg < 0x0C)
        {
            port_byte_out(base + reg - 0x06, data);
        }
        else if (reg < 0x0E)
        {
            port_byte_out(ctrl + reg - 0x0A, data);
        }
        else if (reg < 0x16)
        {
            port_byte_out(bmide + reg - 0x0E, data);
        }
        else
        {
            assert(false);
            __builtin_unreachable();
        }

        if (reg > 0x07 && reg < 0x0C)
        {
            write(IDE_REG_CONTROL, this->nIEN);
        }
    }

    void ChannelRegister_t::readBuffer(const uint16_t reg, uint32_t* buf, uint32_t dwordCount)
    {
        if (reg > 0x07 && reg < 0x0C)
        {
            write(IDE_REG_CONTROL, 0x80 | this->nIEN);
        }

        unsigned short port;


        if (reg < 0x08)
        {
            port = base + reg - 0x00;
        }
        else if (reg < 0x0C)
        {
            port = base + reg - 0x06;
        }
        else if (reg < 0x0E)
        {
            port = ctrl + reg - 0x0A;
        }
        else if (reg < 0x16)
        {
            port = bmide + reg - 0x0E;
        }
        else
        {
            assert(false);
            __builtin_unreachable();
        }

        for (uint32_t i = 0; i < dwordCount; ++i)
        {
            buf[i] = port_long_in(port);
        }



        if (reg > 0x07 && reg < 0x0C)
        {
            write(IDE_REG_CONTROL, this->nIEN);
        }
    }

    unsigned char ChannelRegister_t::poll(const bool advCheck)
    {
        for (int i = 0; i < 4; ++i)
        {
            read(IDE_REG_ALT_STATUS);
        }

        while (read(IDE_REG_STATUS) & IDE_STATE_BUSY) ;

        if (advCheck)
        {
            unsigned char state = read(IDE_REG_STATUS);

            if (state & IDE_STATE_ERROR)
            {
                unsigned char err = read(IDE_REG_ERROR);
                return err;
                return 2;   // Read the error register?
            }

            if (state & IDE_STATE_FAULT)
            {
                return 1;
            }

            if ((state & IDE_STATE_REQUEST_READY) == 0)
            {
                return 3;
            }
        }

        return 0;
    }

    void ChannelRegister_t::select(const unsigned char role)
    {
        assert(role == IDE_MASTER || role == IDE_SLAVE);
        write(IDE_REG_DEVICE_SELECT, role);
    }












    void IDEDevice::Initialize()
	{
        assert(!__initted);
		auto pciDev = PCI::GetDevice(PCIVendorID::ATA, PCIDeviceID::ATA, PCIType::Null);

		if (pciDev == PCI::NULL_DEVICE)
		{
			assert(false);
			pciDev = PCI::GetDevice(PCIVendorID::ATA, PCIDeviceID::PIIX4, PCIType::Null);
		}

		ASSERT(pciDev != PCI::NULL_DEVICE);

		auto bar0 = pciDev.read(PCIRegister::BAR0) + 0x1F0;
		auto bar1 = pciDev.read(PCIRegister::BAR1) + 0x1F0;
		auto bar2 = pciDev.read(PCIRegister::BAR2) + 0x1F0;
		auto bar3 = pciDev.read(PCIRegister::BAR3) + 0x1F0;
		auto bar4 = pciDev.read(PCIRegister::BAR4);

		ASSERT(bar0 == 0x1F0);




        Channels[IDE_PRIMARY].base = (bar0 & 0xFFFFFFFC) + 0x1F0 * (!bar0);
        Channels[IDE_PRIMARY].ctrl = (bar1 & 0xFFFFFFFC) + 0x3F6 * (!bar1);
        Channels[IDE_SECONDARY].base = (bar2 & 0xFFFFFFFC) + 0x170 * (!bar2);
        Channels[IDE_SECONDARY].ctrl = (bar3 & 0xFFFFFFFC) + 0x376 * (!bar3);

        Channels[IDE_PRIMARY].bmide = (bar4 & 0xFFFFFFFC);
        Channels[IDE_SECONDARY].bmide = (bar4 & 0xFFFFFFFC) + 8;

        // Disable IRQs for now
        Channels[IDE_PRIMARY].nIEN = 1;
        Channels[IDE_SECONDARY].nIEN = 1;
        Channels[IDE_PRIMARY].write(IDE_REG_CONTROL, 2);
        Channels[IDE_SECONDARY].write(IDE_REG_CONTROL, 2);



		for (unsigned char channel_index = 0; channel_index < 2; ++channel_index)
		{
			for (unsigned char role_index = 0; role_index < 2; ++role_index)
			{
				ChannelRegister_t* channel = &Channels[channel_index];
                unsigned char status;
                unsigned char ata_err = 0;
                unsigned char role;
                Interface type = Interface::ATA;
                if (role_index == 0)
                {
                    role = IDE_MASTER;
                }
                else
                {
                    role = IDE_SLAVE;
                }


				channel->select(role);
                usleep(1000);

                channel->write(IDE_REG_COMMAND, ATA_CMD_IDENTIFY);
                usleep(1000);



                if (channel->read(IDE_REG_STATUS) == 0)
                {
                    // No device connected to this slot
                    continue;
                }
    
                while (true)
                {
                    status = channel->read(IDE_REG_STATUS);

                    if (status & IDE_STATE_ERROR)
                    {
                        // Not an ATA device
                        ata_err = 1;
                        break;
                    }

                    if (!(status & IDE_STATE_BUSY) && (status & IDE_STATE_REQUEST_READY))
                    {
                        // Looks like an ATA
                        break;
                    }
                }

                if (ata_err)
                {
                    unsigned char cl = channel->read(ATA_REG_LBA_LOW);
                    unsigned char ch = channel->read(ATA_REG_LBA_MID);

                    if (cl == 0x14 && ch == 0xEB)
                    {
                        type = Interface::ATAPI;
                    }
                    else if (cl == 0x69 && ch == 0x96)
                    {
                        type = Interface::ATAPI;
                    }
                    else
                    {
                        // Unknown or missing device
                        continue;
                    }
                }

                unsigned char buf[2048] = {0};

                channel->readBuffer(IDE_REG_DATA, reinterpret_cast<uint32_t*>(buf), 128);

                unsigned short signature = *((unsigned short*)(buf + ATA_IDENTIFY_DEVICE_TYPE));
                unsigned short capabilities = *((unsigned short*)(buf + ATA_IDENTIFY_CAPABILITIES));
                unsigned int cmd_sets = *((unsigned int*)buf + (ATA_IDENTIFY_COMMAND_SETS));

                IDEDevice* device = IDEDeviceFactory::Make(type, channel, role);

                if (!device)
                {
                    // Couldn't determine a type/couldn't create a suitable instance
                    continue;
                }

                device->signature = signature;
                device->features = capabilities;
                device->command_sets = cmd_sets;

                Devices[2*channel_index + role_index] = device;

                if (device->command_sets & (1 << 26))
                {
                    device->sector_count = *((unsigned int*)(buf + ATA_IDENTIFY_MAX_LBA_EXT));
                }
                else
                {
                    device->sector_count = *((unsigned int*)(buf + ATA_IDENTIFY_MAX_LBA));
                }


                for (unsigned int k = 0; k < 40; k += 2)
                {
                    device->model[k] = buf[ATA_IDENTIFY_MODEL + k + 1];
                    device->model[k+1] = buf[ATA_IDENTIFY_MODEL + k];
                }

                device->model[40] = '\0';
			}
		}

		__initted = true;
	}





    IDEDevice::IDEDevice(ChannelRegister_t* channel, unsigned char role) : channel(channel), role(role)
    {

    }



    IDEDevice* IDEDevice::Get(const unsigned char channel, const unsigned char role)
    {
        if (!__initted)
        {
            Initialize();
        }
        
        assert(channel == IDE_PRIMARY || channel == IDE_SECONDARY);
        assert(role == IDE_MASTER || role == IDE_SLAVE);

        int index = 0;
        if (channel == IDE_SECONDARY)
        {
            index += 2;
        }

        if (role == IDE_SLAVE)
        {
            ++index;
        }

        return Devices[index];
    }

















    IDEDevice* IDEDeviceFactory::Make(Interface type, ChannelRegister_t* channel, unsigned char role)
    {
        switch (type)
        {
            case Interface::ATA:
                return new ATA::ATADevice(channel, role);
            
            case Interface::ATAPI:
                //return new ATA::ATAPIDevice(channel, role);

            default:
                return nullptr;
        }
    }

}
}