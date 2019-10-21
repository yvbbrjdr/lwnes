#include <cstdio>
#include <cstdlib>

#include "core/nes.h"

using namespace std;

int main(int argc, char *argv[])
{
    try {
        if (argc != 2) {
            fprintf(stderr, "usage: %s rom\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        NES nes;
        nes.load_rom(argv[1]);
        nes.start();
        return 0;
    } catch(const exception& e) {
        fprintf(stderr, "fatal: %s\n", e.what());
        exit(EXIT_FAILURE);
    }
}
