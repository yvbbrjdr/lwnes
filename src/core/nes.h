#ifndef NES_H
#define NES_H

#include "cpu.h"
#include "dma.h"
#include "rom.h"

class NES {
private:
    DMA dma;
    ROM rom;
    CPU cpu;
public:
    NES();
    void load_rom(const std::string &filename);
    void start();
};

#endif // NES_H
