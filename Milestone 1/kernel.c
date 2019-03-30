#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 16
#define MAX_FILENAME 12
#define MAX_SECTORS 20
#define DIR_ENTRY_LENGTH 32
#define MAP_SECTOR 1
#define DIR_SECTOR 2
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define NOT_FOUND -1
#define INSUFFICIENT_DIR_ENTRIES -1
#define EMPTY 0x00
#define USED 0xFF

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int mod(int a, int b);
int div(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length);
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);
void printLogo();
void readLogo(char *string, int y, char color);

char input[100];
int *success;

int main() {
	makeInterrupt21();
	//Bonus 1
	printLogo();
	//printString("Press any key to continue\0");
	//interrupt(0x16, 0, 0, 0, 0);	

	//Bonus 2
	interrupt(0x21,0x6,"kalku\0",0x2000,success);
	
	//Get Code
	//interrupt(0x21,0x6,"keyproc\0",0x2000,success);
	
	//Show Code
	//interrupt(0x21,0x4,input,"key.txt\0", success);
	//interrupt(0x21,0x0,input);
	while (1);
}

void printLogo()
{
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


void handleInterrupt21 (int AX, int BX, int CX, int DX) {
	switch (AX) {
		case 0x0:
			printString(BX);
			break;
		case 0x1:
			readString(BX);
			break;
		case 0x2:
			readSector(BX, CX);
			break;
		case 0x3:
			writeSector(BX, CX);
			break;
		case 0x4:
			readFile(BX, CX, DX);
			break;
		case 0x5:
			writeFile(BX, CX, DX);
			break;
		case 0x6:
			executeProgram(BX, CX, DX);
			break;
		default:
			printString("Invalid interrupt\0");
	}
}

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

void readString(char *string) {
	int i;
	char c,enter,backspace;
	backspace = 0x8;
	enter = 0xd;
	i = 0;
   	while(1){
		c = interrupt(0x16, 0, 0, 0, 0);
		if(c == enter){		
			interrupt(0x10, 0xE*256+0xa, 0, 0, 0); 
			interrupt(0x10, 0xE*256+0xd, 0, 0, 0);
			//string[i] = 0xa;
			//i++;                   
			string[i] = 0x0;                    
			break;
		
		}else if(c == backspace){
			interrupt(0x10, 0xE*256+c, 0, 0, 0);     
			interrupt(0x10, 0xE*256+0x20, 0, 0, 0);  
			interrupt(0x10, 0xE*256+c, 0, 0, 0);     
			i--;
		}else if (i < 100) {
			interrupt(0x10, 0xE*256+c, 0, 0, 0);  
			string[i] = c;
			i++;
		}
	}
}

int mod(int a, int b) {
	while(a >= b) {
		a = a - b;
	}
	return a;
}

int div(int a, int b) {
	int q = 0;
	while(q*b <= a) {
		q = q+1;
	}
	return q-1;
}

void readSector(char *buffer, int sector) {
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void readFile(char *buffer, char *filename, int *success) {
	int i, j, idx, z;
	int check;
	char dir[SECTOR_SIZE];
	readSector(dir, DIR_SECTOR);
	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
		check = 1;
		for (j = 0; j < MAX_FILENAME && filename[j]!='\0'; j++) {
			int a,b;
			a=filename[j];
			b=dir[i+j];
			if (a != b) {
				check = 0;
				break;
			}
		}
		if (check == 1) {
			idx=i;
			break;
		}
	}

	if (check == 0) {
		*success = FALSE;
		return;
	} else {
		for (j = 0; j < MAX_SECTORS; j++) {
			readSector(buffer + j * SECTOR_SIZE, dir[idx+j+12]);
		}
		*success = TRUE;
		return;
	}
	
}

void clear(char *buffer, int length) {
	int i;
	for(i = 0; i < length; ++i) {
		buffer[i] = EMPTY;
	}
}
​
void writeFile(char *buffer, char *filename, int *sectors) {
	char map[SECTOR_SIZE];
	char dir[SECTOR_SIZE];
	char sectorBuffer[SECTOR_SIZE];
	int dirIndex;

	readSector(map, MAP_SECTOR);
	readSector(dir, DIR_SECTOR);

	for (dirIndex = 0; dirIndex < MAX_FILES; ++dirIndex) {
		if (dir[dirIndex * DIR_ENTRY_LENGTH] == '\0') {
			break;
		}
	}

	if (dirIndex < MAX_FILES) {
		int i, j, sectorCount;
		for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
			if (map[i] == EMPTY) {
				++sectorCount;
			}
		}

		if (sectorCount < *sectors) {
			*sectors = INSUFFICIENT_SECTORS;
			return;
		} else {
			clear(dir + dirIndex * DIR_ENTRY_LENGTH, DIR_ENTRY_LENGTH);
			for (i = 0; i < MAX_FILENAME; ++i) {
				if (filename[i] != '\0') {
					dir[dirIndex * DIR_ENTRY_LENGTH + i] = filename[i];
				} else {
					break;
				}
			}
			for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
				if (map[i] == EMPTY) {
					map[i] = USED;
					dir[dirIndex * DIR_ENTRY_LENGTH + MAX_FILENAME + sectorCount] = i;
					clear(sectorBuffer, SECTOR_SIZE);
					for (j = 0; j < SECTOR_SIZE; ++j) {
						sectorBuffer[j] = buffer[sectorCount * SECTOR_SIZE + j];
					}
					writeSector(sectorBuffer, i);
					++sectorCount;
				}
			}
		}
	} else {
		*sectors = INSUFFICIENT_DIR_ENTRIES;
		return;
	}
	writeSector(map, MAP_SECTOR);
	writeSector(dir, DIR_SECTOR);
}

void executeProgram(char *filename, int segment, int *success){
	int i;
	char buffer[MAX_SECTORS * SECTOR_SIZE];
	readFile(buffer, filename, success);
	if (*success == TRUE)
	{
		printString("Success");
		for (i = 0; i < MAX_SECTORS * SECTOR_SIZE; i++)
		{
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
	}
	else if (*success == FALSE)
	{
		printString("Failure");		
	}
}
