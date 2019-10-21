#ifndef DMA_H
#define DMA_H

#include "memory.h"

class DMA {
private:
    std::vector<Memory> memories;
    Memory &resolve_addr(uint16_t addr);
public:
    DMA();
    uint8_t read(uint16_t addr);
    uint16_t read_dword(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void load_cartridge(const std::vector<uint8_t> &data);
};

#endif // DMA_H
