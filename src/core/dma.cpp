#include "dma.h"

#include <stdexcept>

using namespace std;

Memory &DMA::resolve_addr(uint16_t addr)
{
    for (auto iter = memories.begin(); iter != memories.end(); ++iter)
        if (iter->addr_in_range(addr))
            return *iter;
    throw runtime_error("address not in range: " + to_string(addr));
}

DMA::DMA()
{
    memories.emplace_back(0x0000, 0x1FFF, 0x0800); // RAM
    memories.emplace_back(0x2000, 0x3FFF, 0x0008); // PPU
    memories.emplace_back(0x4000, 0x4017, 0x0018); // APU & IO
    memories.emplace_back(0x4020, 0xFFFF, 0xBFE0); // Cartridge
}

uint8_t DMA::read(uint16_t addr)
{
    return resolve_addr(addr).read(addr);
}

uint16_t DMA::read_dword(uint16_t addr)
{
    return resolve_addr(addr).read_dword(addr);
}

void DMA::write(uint16_t addr, uint8_t data)
{
    resolve_addr(addr).write(addr, data);
}

void DMA::load_cartridge(const vector<uint8_t> &data)
{
    memories[3].load(data);
}
