#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>

class Memory {
private:
    uint16_t start_addr;
    uint16_t end_addr;
    std::vector<uint8_t> data;
    uint16_t resolve_addr(uint16_t addr);
public:
    Memory(uint16_t start_addr, uint16_t end_addr, uint16_t length);
    uint16_t length();
    bool addr_in_range(uint16_t addr);
    uint8_t read(uint16_t addr);
    uint16_t read_dword(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void load(const std::vector<uint8_t> &data);
    const std::vector<uint8_t> &dump();
};

#endif // MEMORY_H
