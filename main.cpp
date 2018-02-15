#include "nes.hpp"

int main(int argc, char *argv[])
{

    NES nes;

    nes.loadCartridge(".\\test\\mytest.nes");

    nes.debugConsole("NES> ");



    return 0;
}
