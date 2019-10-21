#include "rom.h"

#include <fstream>
#include <stdexcept>

using namespace std;

void ROM::load_file(const string &filename)
{
    ifstream file(filename, ifstream::binary);
    if (file) {
        load(file);
        file.close();
    } else {
        throw runtime_error("unable to open rom file");
    }
}

void ROM::load(istream &stream)
{
    vector<uint8_t> header(16, 0);
    stream.read((char *) header.data(), 16);
    if (!stream)
        throw runtime_error("invalid NES rom file");
    if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A)
        throw runtime_error("invalid NES rom file");
    uint32_t prg_rom_len = header[4] << 14;
    uint32_t chr_rom_len = header[5] << 13;
    if ((header[6] & 0x04) == 0x04) {
        trainer.resize(512, 0);
        stream.read((char *) trainer.data(), 512);
        if (!stream)
            throw runtime_error("invalid NES rom file");
    } else {
        trainer.clear();
    }
    prg_rom.resize(prg_rom_len, 0);
    stream.read((char *) prg_rom.data(), prg_rom_len);
    if (!stream)
        throw runtime_error("invalid NES rom file");
    chr_rom.resize(chr_rom_len, 0);
    stream.read((char *) chr_rom.data(), chr_rom_len);
    if (!stream)
        throw runtime_error("invalid NES rom file");
}

vector<uint8_t> ROM::to_cartridge()
{
    vector<uint8_t> ret(8160, 0);
    ret.insert(ret.end(), chr_rom.begin(), chr_rom.end());
    ret.insert(ret.end(), prg_rom.begin(), prg_rom.end());
    return ret;
}
