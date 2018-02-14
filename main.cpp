#include "nes.hpp"

int main(int argc, char *argv[])
{

    NES nes;

    nes.loadCartridge(".\\test\\nestest.nes");
    nes.debugC6502Console();


    return 0;
}
