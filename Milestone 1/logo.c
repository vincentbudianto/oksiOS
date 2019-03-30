void readLogo(char *string, int y, char color);
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);

main( void ) {
	readLogo("     * ***       *                                         * ***           *******                       \0", 1, 0x5);
	readLogo("    *  ****     **                        *               *  ****         *       ***                    \0", 2, 0xD);
	readLogo("   *  *  ***    **                       ***             *  *  ***       *         **                    \0", 3, 0x5);
	readLogo("  *  **   ***   **                        *             *  **   ***      **        *                     \0", 4, 0xD);
	readLogo(" *  ***    ***  **             ****                    *  ***    ***      ***                            \0", 5, 0x5);
	readLogo("**   **     **  **  ***       * **** *  ***           **   **     **     ** ***                          \0", 6, 0xD);
	readLogo("**   **     **  ** * ***     **  ****    ***          **   **     **      *** ***                        \0", 7, 0x5);
	readLogo("**   **     **  ***   *     ****          **          **   **     **        *** ***                      \0", 8, 0xD);
	readLogo("**   **     **  **   *        ***         **    ****  **   **     **          *** ***                    \0", 9, 0x5);
	readLogo("**   **     **  **  *           ***       **          **   **     **            ** ***                   \0", 10, 0xD);
	readLogo(" **  **     **  ** **             ***     **           **  **     **             ** **                   \0", 11, 0x5);
	readLogo("  ** *      *   ******       ****  **     **            ** *      *               * *                    \0", 12, 0xD);
	readLogo("   ***     *    **  ***     * **** *      **             ***     *      ***        *                     \0", 13, 0x5);
	readLogo("    *******     **   *** *     ****       *** *           *******      *  *********                      \0", 14, 0xD);
	readLogo("      ***        **   ***                  ***              ***       *     *****                        \0", 15, 0x5);
	readLogo("                                                                      *                                  \0", 16, 0xD);
	readLogo("                                                                       **                                \0", 17, 0x5);

	/*readLogo("                         #       #                                                                       \0", 20, 0x3);
	readLogo("                         #       #                                                                       \0", 21, 0xB);
	readLogo("                                #                                                                        \0", 22, 0x3);
	readLogo("                               #                                                                         \0", 23, 0xB);
	readLogo("                             ##                                                                          \0", 24, 0x3);
	readLogo("                           ##                                                                            \0", 25, 0xB);
	readLogo("                         ##                                                                              \0", 26, 0x3);

	readLogo("                                     #####                                                               \0", 29, 0x2);
	readLogo("                                    #     #                                                              \0", 30, 0xa);
	readLogo("                                          #                                                              \0", 31, 0x2);
	readLogo("                                     #####                                                               \0", 32, 0xa);
	readLogo("                                    #                                                                    \0", 33, 0x2);
	readLogo("                                    #                                                                    \0", 34, 0xa);
	readLogo("                                    #######                                                              \0", 35, 0x2);

	readLogo("                                                ######                                                   \0", 38, 0xE);
	readLogo("                                              ##########                                                 \0", 40, 0xF);
	readLogo("                                                   #                                                     \0", 41, 0xE);
	readLogo("                                                   #                                                     \0", 42, 0xF);
	readLogo("                                                  #                                                      \0", 43, 0xE);
	readLogo("                                                ##                                                       \0", 44, 0xF);*/
	
	printString("Press any key to continue\0");
	interrupt(0x16, 0, 0, 0, 0);
}

void readLogo(char *string, int y, char color)
{
	int x;
	char c;
	
	x = 0;
	c = string[x];

   	while(c != '\0' && x<70)
   	{
		c = string[x];
		putInMemory(0xB000, (0x8000 + ((80 * y) + x) * 2), c);
		putInMemory(0xB000, (0x8001 + ((80 * y) + x) * 2), color);
		x++;
	}
}


void handleInterrupt21 (int AX, int BX, int CX, int DX) {}

void printString(char *string) {
	int c;
	int i;
	i = 0;
	c = string[0];
	while (c != '\0')
	{
		interrupt(0x10, 0xE00 + c, 0, 0, 0);
		i++;
		c = string[i];
	}
	interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
	interrupt(0x10, 0xE*256+0xd, 0, 0, 0);
}

