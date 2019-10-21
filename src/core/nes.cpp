#include "nes.h"

using namespace std;

NES::NES() : cpu(dma) {}

void NES::load_rom(const string &filename)
{
    rom.load_file(filename);
    dma.load_cartridge(rom.to_cartridge());
    cpu.reset();
}

void NES::start()
{
    cpu.start();
}
