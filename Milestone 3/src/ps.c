#include "proc.h"

#ifndef NULL
#define NULL 0x00
#endif

#ifndef MAIN
#define MAIN
#endif

int main()
{
    
    char curdir;

    enableInterrupts();

    interrupt(0x21, 0x21, &curdir, 0, 0);


    interrupt(0x21, 0x00, "Mengecek semua PCB yang ada", 0, 0);
    interrupt(0x10, 0xE*256+'\n', 0, 0, 0); 
	interrupt(0x10, 0xE*256+'\r', 0, 0, 0);	

    // Bagian pembacaan PCB, misalnya gagal pindahin ke kernel
    interrupt(0x21, 0x37, 0, 0, 0);

    interrupt(0x21, curdir << 8 | 0x07,0, 0, 0);

    return 0;
}