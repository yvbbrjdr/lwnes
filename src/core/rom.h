#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include <string>
#include <vector>

class ROM {
private:
    std::vector<uint8_t> trainer;
    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
public:
    void load_file(const std::string &filename);
    void load(std::istream &stream);
    std::vector<uint8_t> to_cartridge();
};

#endif // ROM_H
