#include "memory.h"

#include <stdexcept>

using namespace std;

uint16_t Memory::resolve_addr(uint16_t addr)
{
    if (!addr_in_range(addr))
        throw runtime_error("address not in range: " + to_string(addr));
    return (addr - start_addr) % data.size();
}

Memory::Memory(uint16_t start_addr, uint16_t end_addr, uint16_t length) :
    start_addr(start_addr), end_addr(end_addr), data(length, 0) {}

uint16_t Memory::length()
{
    return data.size();
}

bool Memory::addr_in_range(uint16_t addr)
{
    return start_addr <= addr && addr <= end_addr;
}

uint8_t Memory::read(uint16_t addr)
{
    return data[resolve_addr(addr)];
}

uint16_t Memory::read_dword(uint16_t addr)
{
    if (!addr_in_range(addr + 1))
        throw runtime_error("address not in range: " + to_string(addr + 1));
    uint16_t index = resolve_addr(addr);
    return data[index] | (data[index + 1] << 8);
}

void Memory::write(uint16_t addr, uint8_t data)
{
    this->data[resolve_addr(addr)] = data;
}

void Memory::load(const vector<uint8_t> &data)
{
    uint16_t length = this->data.size();
    this->data = data;
    this->data.resize(length, 0);
}

const vector<uint8_t> &Memory::dump()
{
    return data;
}
