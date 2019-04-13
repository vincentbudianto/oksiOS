#define MAIN
#include "proc.h"

#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 32
#define MAX_FILENAME 15
#define MAX_SECTORS 16
#define DIR_ENTRY_LENGTH 16
#define MAP_SECTOR 0x100
#define DIR_SECTOR 0x101
#define FILE_SECTOR 0x102
#define SECTORS 0x103
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS -3
#define SUCCESS 0
#define NOT_FOUND -1
#define ALREADY_EXISTS -2
#define INSUFFICIENT_FILE_ENTRIES -3
#define INSUFFICIENT_DIR_ENTRIES -3
#define EMPTY 0x00
#define USED 0xFF
#define NUL '\0'
#define ARGS_SECTOR 512
#define INSUFFICIENT_SEGMENTS -3
#define CTRL_C 0x03
#define CTRL_Z 0x1A

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void handleTimerInterrupt(int segment, int stackPointer);
void printString(char *string);
void readString(char *string, int disableProcessControls);
void yieldControl();
void sleep();
void pauseProcess(int segment, int *result);
void resumeProcess(int segment, int *result);
void killProcess(int segment, int *result);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void clear(char *buffer, int length);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void executeProgram (char *path, int *result, char parentIndex);
void executeProgramNoSleep(char *path, int *result, char parentIndex);
void terminateProgram (int *result);
void terminateProgramNoResume(int *result);
void printProcTable();
void putArgs (char curdir, char argc, char argv[81][81]);
void getCurdir (char *curdir);
void getArgc (char *argc);
void getArgv (char index, char *argv);
int mod(int a, int b);
int div(int a, int b);
void printInt(int x);

// Atribut
char input[100];
int *success;

int main()
{
	int i;
	char curdir, argc, argv[4][16];
	
	initializeProcStructures();
	makeInterrupt21();
	makeTimerInterrupt();

	interrupt(0x21, 0x20, 0xFF, 0, 0);
	interrupt(0x21, 0xFF << 8 | 0x6, "shell\0", 0x2000, success);

	while (1);
}


void handleInterrupt21 (int AX, int BX, int CX, int DX)
{
	char AL, AH;
	
	AL = (char) (AX);
	AH = (char) (AX >> 8);
	
	switch (AL)
	{
		case 0x00:
			printString(BX);
			break;
		case 0x01:
			readString(BX, CX);
			break;
		case 0x02:
			readSector(BX, CX);
			break;
		case 0x03:
			writeSector(BX, CX);
			break;
		case 0x04:
			readFile(BX, CX, DX, AH);
			break;
		case 0x05:
			writeFile(BX, CX, DX, AH);
			break;
		case 0x06:
			executeProgram(BX, CX, AH);
			break;
		case 0x07:
			printString("Stopped");
			terminateProgram(BX);
			break;
		case 0x08:
			handleTimerInterrupt(BX, CX);
			break;
		case 0x20:
			putArgs(BX, CX, DX);
			break;
		case 0x21:
			getCurdir(BX);
			break;
		case 0x22:
			getArgc(BX);
			break;
		case 0X23:
			getArgv(BX, CX);
			break;
		case 0x30:
			yieldControl();
			break;
		case 0x31:
			sleep();
			break;
		case 0x32:
			pauseProcess(BX, CX);
			break;
		case 0x33:
			resumeProcess(BX, CX);
			break;
		case 0x34:
			killProcess(BX, CX);
			break;
		case 0x35:
			executeProgramNoSleep(BX,CX,AH);
			break;
		case 0x36:
			terminateProgramNoResume(BX);
			break;
		case 0x37:
			printProcTable();
			break;
		default:
			printString("Invalid interrupt");
	}
}

void printString(char *string)
{
	int c, i;

	i = 0;
	c = string[0];

	while (c != '\0')
	{
		interrupt(0x10, 0xE00 + c, 0, 0, 0);
		i++;
		c = string[i];
	}
}

void readString(char *string, int disableProcessControls)
{
	int i, result, parentSegment;
	char c, enter, backspace;

	backspace = 0x8;
	enter = 0xd;
	i = 0;
   	
	while (1)
	{
		c = interrupt(0x16, 0, 0, 0, 0);
		
		if (c == enter)
		{		
			interrupt(0x10, 0xE*256+'\n', 0, 0, 0); 
			interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
			string[i] = 0x0;                    
			break;
		}
		else if (disableProcessControls == 0 && c == CTRL_C)
		{
			interrupt(0x10, 0xE*256+'\n', 0, 0, 0); 
			interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
			string[i] = 0x0;
			terminateProgram(&result);
			break;
		}
		else if (disableProcessControls == 0 && c == CTRL_Z)
		{
			interrupt(0x10, 0xE*256+'\n', 0, 0, 0);
			interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
			string[i] = 0x0;
			setKernelDataSegment();
			resumeProcess((running->parentSegment), &result);
			restoreDataSegment();
			sleep();
			break;
		}
		else if (c == backspace)
		{
			if (i > 0)
			{
				interrupt(0x10, 0xE*256+c, 0, 0, 0);     
				interrupt(0x10, 0xE*256+0x20, 0, 0, 0);  
				interrupt(0x10, 0xE*256+c, 0, 0, 0);     
				i--;
			}
		}
		else if (i < 100)
		{
			interrupt(0x10, 0xE*256+c, 0, 0, 0);  
			string[i] = c;
			i++;
		}
	}
}

void handleTimerInterrupt(int segment, int stackPointer)
{
	struct PCB *currPCB, *nextPCB;
	
	setKernelDataSegment();
	currPCB = getPCBOfSegment(segment);
	currPCB->stackPointer = stackPointer;
	
	if (currPCB->state != PAUSED)
	{
		currPCB->state = READY;
		addToReady(currPCB);
	}

	do
	{
		nextPCB = removeFromReady();
	} while (nextPCB != NULL && (nextPCB->state == DEFUNCT || nextPCB->state == PAUSED));

	if (nextPCB != NULL)
	{
		nextPCB->state = RUNNING;
		segment = nextPCB->segment;
		stackPointer = nextPCB->stackPointer;
		running = nextPCB;
	}
	else
	{
		running = &idleProc;
	}

	restoreDataSegment();
	returnFromTimer(segment, stackPointer);
}

void yieldControl()
{
	interrupt(0x08, 0, 0, 0, 0);
}

void sleep()
{
	setKernelDataSegment();
	running->state = PAUSED;
	restoreDataSegment();
	yieldControl();
}

void pauseProcess(int segment, int *result)
{
	struct PCB *pcb;
	int res;

	setKernelDataSegment();
	pcb = getPCBOfSegment(segment);
	
	if (pcb != NULL && pcb->state != PAUSED)
	{
		pcb->state = PAUSED;
		res = SUCCESS;
	}
	else
	{
		res = NOT_FOUND;
	}
	
	restoreDataSegment();
	*result = res;
}

void resumeProcess(int segment, int *result)
{
	struct PCB *pcb;
	int res;
	
	setKernelDataSegment();
	pcb = getPCBOfSegment(segment);
	
	if (pcb != NULL && pcb->state == PAUSED)
	{
		pcb->state = READY;
		addToReady(pcb);
		res = SUCCESS;
	}
	else
	{
		res = NOT_FOUND;
	}
	
	restoreDataSegment();
	*result = res;
}

void killProcess(int segment, int *result)
{
	struct PCB *pcb;
	int res;
	
	setKernelDataSegment();
	pcb = getPCBOfSegment(segment);
	
	if (pcb != NULL)
	{
		releaseMemorySegment(pcb->segment);
		releasePCB(pcb);
		res = SUCCESS;
	}
	else
	{
		res = NOT_FOUND;
	}

	restoreDataSegment();
	*result = res;
}

void readSector(char *buffer, int sector)
{
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector)
{
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
	int a, b, c, i, j, idx, jdx, z, isFile, check, locSectors;
	char dirs[SECTOR_SIZE], files[SECTOR_SIZE], sectors[SECTOR_SIZE];

	readSector(dirs, DIR_SECTOR);
	readSector(files, FILE_SECTOR);
	j = 0;

	while (path[j] != '\0' && path[j] != '/')
	{
		j++;
	}
	
	if (path[j] == '\0')
	{
		isFile = 1;
	}
	else if (path[j] == '/')
	{
		isFile = 0;
	}

	for (i = 0; i < SECTOR_SIZE; i += DIR_ENTRY_LENGTH)
	{
		check = 1;
		
		for (j = 0; path[j] !='\0' && path[j] != '/'; j++)
		{
			a = path[j];
			b = dirs[i+j+1];
			c = files[i+j+1];
			jdx = j;

			if (a != b && isFile == 0)
			{
				check = 0;
				break;
			}
			else if (a != c && isFile == 1)
			{
				check = 0;
				break;
			}
		}

		if (check == 1 && isFile == 0)
		{
			if(dirs[i] == parentIndex)
			{
				idx = div(i,DIR_ENTRY_LENGTH);
				readFile(buffer,path+jdx+2,result,idx);
				return;
			}
			else
			{
				check = 0;
			}
		}
		else if (check == 1 && isFile == 1)
		{
			if (files[i] == parentIndex)
			{
				idx = div(i,DIR_ENTRY_LENGTH);
				break;
			}
			else
			{
				check = 0;
			}
		}
	}

	if (check == 0)
	{
		printString("Not Found\0");
		*result = NOT_FOUND;
		return;
	}
	else
	{
		readSector(sectors, SECTORS);
		locSectors = idx*DIR_ENTRY_LENGTH;

		for (j = 0; j < DIR_ENTRY_LENGTH; j++)
		{
			if (sectors[locSectors + j] == '\0')
			{
				break;
			}
			else
			{
				readSector(buffer + j * SECTOR_SIZE, sectors[locSectors + j]);
			}
		}

		*result = idx;
		return;
	}
}

void clear(char *buffer, int length)
{
	int i;

	for(i = 0; i < length; ++i)
	{
		buffer[i] = EMPTY;
	}
}
​
void writeFile(char *buffer, char *path, int *sectors, char parentIndex)
{
	int a, b, c, i, j, idx, jdx, isFile, check, sectorCount, fileIndex;
	char dirs[SECTOR_SIZE], files[SECTOR_SIZE], map[SECTOR_SIZE], sectorz[SECTOR_SIZE], sectorBuffer[SECTOR_SIZE];

	readSector(dirs, DIR_SECTOR);
	readSector(files, FILE_SECTOR);
	j = 0;

	while(path[j] != '\0' && path[j] != '/')
	{
		j++;
	}
	
	if (path[j] =='\0')
	{
		isFile = 1;
	}
	else if (path[j] == '/')
	{
		isFile = 0;
	}
	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH)
	{
		check = 1;

		for (j = 0; path[j] != '\0' && path[j] != '/'; j++)
		{
			a = path[j];
			b = dirs[i + j + 1];
			c = files[i + j + 1];

			if (a != b && isFile == 0)
			{
				check = 0;
				break;
			}
			else if (a != c && isFile == 1)
			{
				check = 0;
				break;
			}
		}

		if (check == 1 && isFile == 0)
		{
			if (dirs[i] == parentIndex)
			{
				idx = (i >> 4);
				writeFile(buffer,path+j+1,sectors,idx);
				return;
			}
			else
			{
				check = 0;
			}
		}
		else if (check == 1 && isFile == 1)
		{
			if (files[i] == parentIndex)
			{
				idx = i;
				*sectors = ALREADY_EXISTS;
				printString("File already exists");
				return;
			}
		}
	}

	if (check == 0 && isFile == 0)
	{
		*sectors = NOT_FOUND;
		return;
	}
	else
	{
		readSector(sectorz, SECTORS);
		readSector(map, MAP_SECTOR);

		for (fileIndex = 0; fileIndex < MAX_FILES; ++fileIndex)
		{
			if (files[fileIndex * DIR_ENTRY_LENGTH] == '\0' && files[fileIndex * DIR_ENTRY_LENGTH + 1] == '\0')
			{
				break;
			}
		}

		if (fileIndex < MAX_FILES)
		{
			for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i)
			{
				if (map[i] == EMPTY)
				{
					++sectorCount;
				}
			}

			if (sectorCount < *sectors)
			{
				*sectors = INSUFFICIENT_SECTORS;
				return;
			}
			else
			{
				clear(files + fileIndex * DIR_ENTRY_LENGTH, DIR_ENTRY_LENGTH);
				files[fileIndex * DIR_ENTRY_LENGTH] = parentIndex;
			
				for (i = 0; i < MAX_FILENAME; ++i)
				{
					if (path[i] != '\0')
					{
						files[fileIndex * DIR_ENTRY_LENGTH + i +1] = path[i];
					}
					else
					{
						break;
					}
				}

				for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i)
				{
					if (map[i] == EMPTY)
					{
						map[i] = USED;
						sectorz[fileIndex * DIR_ENTRY_LENGTH + sectorCount] = i;
						clear(sectorBuffer, SECTOR_SIZE);
					
						for (j = 0; j < SECTOR_SIZE; ++j)
						{
							sectorBuffer[j] = buffer[sectorCount * SECTOR_SIZE + j];
						}

						writeSector(sectorBuffer, i);
						++sectorCount;
					}
				}
			}
		}
		else
		{
			*sectors = INSUFFICIENT_DIR_ENTRIES;
			return;
		}
		
		writeSector(map, MAP_SECTOR);
		writeSector(files, FILE_SECTOR);
		writeSector(sectorz, SECTORS);
	}
}

void executeProgram(char *path, int *result, char parentIndex)
{
	struct PCB *pcb;
	int segment, i, fileIndex;
	char buffer[MAX_SECTORS * SECTOR_SIZE];

	readFile(buffer, path, result, parentIndex);
	
	if (*result != NOT_FOUND)
	{
		setKernelDataSegment();
		segment = getFreeMemorySegment();
		restoreDataSegment();
		fileIndex = *result;
		
		if (segment != NO_FREE_SEGMENTS)
		{
			setKernelDataSegment();
			pcb = getFreePCB();
			pcb->index = fileIndex;
			pcb->state = STARTING;
			pcb->segment = segment;
			pcb->stackPointer = 0xFF00;
			pcb->parentSegment = running->segment;
			addToReady(pcb);
			restoreDataSegment();
			
			for (i = 0; i < SECTOR_SIZE * MAX_SECTORS; i++)
			{
				putInMemory(segment, i, buffer[i]);
			}
			
			initializeProgram(segment);
			sleep();
		}
		else
		{
			*result = INSUFFICIENT_SEGMENTS;
		}
	}
}

void executeProgramNoSleep(char *path, int *result, char parentIndex)
{
	struct PCB *pcb;
	int segment, i, fileIndex;
	char buffer[MAX_SECTORS * SECTOR_SIZE];

	readFile(buffer, path, result, parentIndex);
	
	if (*result != NOT_FOUND)
	{
		setKernelDataSegment();
		segment = getFreeMemorySegment();
		restoreDataSegment();
		fileIndex = *result;
		
		if (segment != NO_FREE_SEGMENTS)
		{
			setKernelDataSegment();
			pcb = getFreePCB();
			pcb->index = fileIndex;
			pcb->state = STARTING;
			pcb->segment = segment;
			pcb->stackPointer = 0xFF00;
			pcb->parentSegment = running->segment;
			addToReady(pcb);
			restoreDataSegment();
			
			for (i = 0; i < SECTOR_SIZE * MAX_SECTORS; i++)
			{
				putInMemory(segment, i, buffer[i]);
			}
			
			initializeProgram(segment);
		}
		else
		{
			*result = INSUFFICIENT_SEGMENTS;
		}
	}
}

void terminateProgram(int *result)
{
	int parentSegment;

	setKernelDataSegment();
	parentSegment = running->parentSegment;
	releaseMemorySegment(running->segment);
	releasePCB(running);
	restoreDataSegment();
	
	if (parentSegment != NO_PARENT)
	{
		resumeProcess(parentSegment, result);
	}
	
	yieldControl();
}

void terminateProgramNoResume(int *result)
{
	int parentSegment;
	
	setKernelDataSegment();
	parentSegment = running->parentSegment;
	releaseMemorySegment(running->segment);
	releasePCB(running);
	restoreDataSegment();
	yieldControl();
}

void printProcTable()
{
	int i, x;

	setKernelDataSegment();
	
	for (i = 0; i &lt; 8; i++)
	{
		interrupt(0x21, 0x00, "PCB ", 0, 0);
		x = ((pcbPool[i].segment >> 12)-2);
		printInt(x);
		interrupt(0x21, 0x00, ": ", 0, 0);

		if (pcbPool[i].state == 0)
		{
			interrupt(0x21, 0x00, "DEFUNCT", 0, 0);
		}
		else if (pcbPool[i].state == 1)
		{
			interrupt(0x21, 0x00, "RUNNING", 0, 0);
		}
		else if (pcbPool[i].state == 2)
		{
			interrupt(0x21, 0x00, "STARTING", 0, 0);
		}
		else if (pcbPool[i].state == 3)
		{
			interrupt(0x21, 0x00, "READY", 0, 0);
		}
		else
		{
			interrupt(0x21, 0x00, "PAUSED", 0, 0);
		}

		interrupt(0x10, 0xE*256+'\n', 0, 0, 0);
		interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
	}

	restoreDataSegment();
}

int	mod(int a, int b)
{
	while (a >= b)
	{
		a = a - b;
	}

	return a;
}

int div(int a, int b)
{
	int q = 0;

	while (q * b <= a)
	{
		q = q + 1;
	}

	return q - 1;
}

void putArgs (char curdir, char argc, char argv[81][81]) {
	char args[SECTOR_SIZE];
	int i, j, p;

	clear(args, SECTOR_SIZE);
	args[0] = curdir;
	args[1] = argc;
	i = 0;
	j = 0;   

	for (p = 2; p < ARGS_SECTOR && i < argc; p++)
	{
		args[p] = argv[i][j];

		if (argv[i][j] == '\0')
		{
			i++;
			j = 0;
		}
		else
		{
			j++;
		}
	}

	writeSector(args, ARGS_SECTOR);
}

void getCurdir (char *curdir)
{
	char args[SECTOR_SIZE];

	readSector(args, ARGS_SECTOR);
	*curdir = args[0];
}

void getArgc (char *argc)
{
	char args[SECTOR_SIZE];

	readSector(args, ARGS_SECTOR);
	*argc = args[1];
}

void getArgv (char index, char *argv)
{
	char args[SECTOR_SIZE];
	int i, j, p;

	readSector(args, ARGS_SECTOR);
	i = 0;
	j = 0; 
	
	for (p = 2; p < ARGS_SECTOR; p++)
	{
		if (i == index)
		{
			argv[j] = args[p];
			++j;
		}  
		
		if (args[p] == '\0')
		{
			if (i == index)
			{
				break;
			}
			else
			{
				++i;
			}
		}
	}
}

void printInt(int x)
{
	int a, b;
	
	a = 1;

	if (x > 0)
	{
		while (div(x, a) != 0)
		{
			a *= 10;
		}
		
		a /= 10;
		
		while (a != 0)
		{
			b = div(x, a);
			interrupt(0x10, 0xE00 + (b + '0'), 0, 0, 0);
			x = x - b * a;
			a /= 10;
		}
	}
	else if (x == 0)
	{
		interrupt(0x10, 0xE00 + '0', 0, 0, 0);
	}
	else if (x < 0)
	{
		x *= -1;
		interrupt(0x10, 0xE00 + '-', 0, 0, 0);
		
		while (div(x, a) != 0)
		{
			a *= 10;
		}
		
		a /= 10;
		
		while (a != 0)
		{
			b = div(x, a);
			interrupt(0x10, 0xE00 + (b + '0'), 0, 0, 0);
			x = x - b * a;
			a /= 10;
		}
	}

	interrupt(0x10, 0xE * 256 + 0xa, 0, 0, 0);
	interrupt(0x10, 0xE * 256 + 0xd, 0, 0, 0);
}
