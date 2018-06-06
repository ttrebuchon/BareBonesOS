#include "AHCI.hh"
#include <drivers/PCI.hh>

namespace Drivers { namespace AHCI
{
    void AHCIDrive::foo()
    {
        PCIDevice_t pci = PCI::ScanByClass(PCIType::SATA);
        assert(pci != PCI::NULL_DEVICE);
    }


}
}