#include "nes.hpp"

int main(int argc, char *argv[])
{

    NES nes;

    nes.loadCartridge(".\\test\\donkeyk.nes");
    nes.debugConsole("NES> ");


    return 0;
}
