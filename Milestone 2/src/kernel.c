#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 32
#define MAX_FILENAME 15
#define MAX_SECTORS 32
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

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int mod(int a, int b);
int div(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void clear(char *buffer, int length);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void executeProgram(char *path, int segment, int *result, char parentIndex);


void terminateProgram (int *result);
void makeDirectory(char *path, int *result, char parentIndex);
void deleteFile(char *path, int *result, char parentIndex);
void deleteDirectory(char *path, int *success, char parentIndex);
void deleteDirectory2 (char *map, char *sectors, char *dirs, char *files, char parentIndex);

void putArgs (char curdir, char argc, char argv[81][81]);
void getCurdir (char *curdir);
void getArgc (char *argc);
void getArgv (char index, char *argv);

void printInt(int x);
//void validatePath(char *path, int *result, char parentIndex);

char input[100];
int *success;

int main() {
	int i;
	char curdir;
	char argc;
	char argv[4][16];

	makeInterrupt21();

	// Milestone 2
	// int i;    
	// char curdir;    
	// char argc;    
	// char argv[4][16];      
	// interrupt(0x21, 0x21, &curdir, 0, 0);    
	// interrupt(0x21, 0x22, &argc, 0, 0);    
	// for (i = 0; i < argc; ++i) {       
	// 	interrupt(0x21, 0x23, i, argv[i], 0);    
	// }
	//printLogo();
	//printString("Press any key to continue\0");
	//interrupt(0x16, 0, 0, 0, 0);
	
	/*interrupt(0x21, 0x21, &curdir, 0, 0);
	interrupt(0x21, 0x22, &argc, 0, 0);
	for (i = 0; i < argc; ++i) {
		interrupt(0x21, 0x23, i, argv[i], 0);
	}
	interrupt(0x21, 0x20, curdir, argc, argv);*/
	interrupt(0x21, 0x20, 0xFF, 0, 0);
	interrupt(0x21, 0xFF << 8 | 0x6, "shell\0", 0x2000, success);
	// Why &success?

	// Milestone 1 ada di bawah
	//Bonus 1

	//Bonus 2
	//interrupt(0x21,0x6,"kalku\0",0x2000,success);
	
	//Get Code
	//interrupt(0x21,0x6,"keyproc\0",0x2000,success);
	
	//Show Code
	//interrupt(0x21,0x4,input,"key.txt\0", success);
	//interrupt(0x21,0x0,input);
	while (1);
}


void handleInterrupt21 (int AX, int BX, int CX, int DX) {
	char AL, AH;
	
	AL = (char) (AX);
	AH = (char) (AX >> 8);
	
	switch (AL) {
		case 0x00:
			printString(BX);
			break;
		case 0x01:
			readString(BX);
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
			executeProgram(BX, CX, DX, AH);
			break;
		case 0x07:
			printString("Stopped");
			terminateProgram(BX);
			break;
		case 0x08:
			makeDirectory(BX, CX, AH);
			break;
		case 0x09:
			deleteFile(BX, CX, AH);
			break;
		case 0x0A:
			deleteDirectory(BX, CX, AH);
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
		default:
			printString("Invalid interrupt");
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

void readFile(char *buffer, char *path, int *result, char parentIndex){
	int i, j, idx, jdx, z, isFile;
	int a,b,c;
	int check;
	int locSectors;
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	char sectors[SECTOR_SIZE];
	readSector(dirs, DIR_SECTOR);
	readSector(files, FILE_SECTOR);
	//Cek path = direktori atau file
	j=0;
	while(path[j]!='\0' && path[j]!='/') {
		j++;
	}
	
	if (path[j]=='\0'){
		isFile=1;
		// barusan isFile = 0
	}
	else if (path[j]=='/'){
		isFile=0;
		// barusan isFile = 1
	}
	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
		check = 1;
		for (j = 0; path[j] !='\0' && path[j]!='/'; j++) {
			a=path[j];
			b=dirs[i+j+1];
			c=files[i+j+1];
			jdx=j;
			if (a != b && isFile == 0) {
				check = 0;
				break;
			}
			else if (a != c && isFile == 1) {
				check = 0;
				break;
			}
		}
		if (check == 1 && isFile == 0) {
			if(dirs[i]==parentIndex){
				idx=div(i,DIR_ENTRY_LENGTH);
				readFile(buffer,path+jdx+2,result,idx);
				// tadi parentIndex
				return;
			}
			else{
				check=0;
			}
		}
		else if (check == 1 && isFile == 1){
			if(files[i]==parentIndex){
				idx=div(i,DIR_ENTRY_LENGTH);
				break;
			}
			else{
				check=0;
			}
		}
	}
	// validasi sektor/file
	if (check == 0) {
		printString("Not Found\0");
		*result = NOT_FOUND;
		return;
	} else {
		readSector(sectors, SECTORS);
		locSectors = idx*DIR_ENTRY_LENGTH;
		for (j = 0; j < DIR_ENTRY_LENGTH; j++) {
			if (sectors[locSectors + j] == '\0')
			{
				break;
			}
			else{
				readSector(buffer + j * SECTOR_SIZE, sectors[locSectors + j]);
			}
			
			//tadi dirs[idx]
		}
		*result = 0;
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
void writeFile(char *buffer, char *path, int *sectors, char parentIndex){
	//Validasi path
	int i, j, idx, jdx, isFile;
	int a,b,c;
	int check;
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	int sectorCount;

	//Selesai validasi path
	char map[SECTOR_SIZE];
	char sectorz[SECTOR_SIZE];
	char sectorBuffer[SECTOR_SIZE];
	int fileIndex;

	readSector(dirs, DIR_SECTOR);
	readSector(files, FILE_SECTOR);
	
	//Cek path = direktori atau file
	j=0;
	while(path[j]!='\0' && path[j]!='/') {
		j++;
	}
	if (path[j]=='\0'){
		isFile=1;
	}
	else if (path[j]=='/'){
		isFile=0;
	}
	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
		check = 1;
		for (j = 0; path[j] !='\0' && path[j]!='/'; j++) {
			a=path[j];
			b=dirs[i+j+1];
			c=files[i+j+1];
			if (a != b && isFile == 0) {
				check = 0;
				break;
			}
			else if (a != c && isFile == 1) {
				check = 0;
				break;
			}
		}
		if (check == 1 && isFile == 0) {
			if(dirs[i]==parentIndex){
				idx = (i >> 4);
				writeFile(buffer,path+j+1,sectors,idx);
				return;
			}
			else{
				check = 0;
			}
		}
		else if (check == 1 && isFile == 1){
			if(files[i]==parentIndex){
				idx=i;
				*sectors= ALREADY_EXISTS;
				printString("File already exists");
				return;
			}
		}
	}
	// validasi sektor/file
	printInt(*sectors);
	//printInt(isFile);
	//printInt(check);
	if (check == 0 && isFile == 0) {
		*sectors = NOT_FOUND;
		return;
	} else {
		
		readSector(sectorz, SECTORS);
		readSector(map, MAP_SECTOR);

		for (fileIndex = 0; fileIndex < MAX_FILES; ++fileIndex) {
			if (files[fileIndex * DIR_ENTRY_LENGTH] == '\0' && files[fileIndex * DIR_ENTRY_LENGTH + 1] == '\0') {
				break;
			}
		}
		printInt(fileIndex);
		if (fileIndex < MAX_FILES) {
			for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
				if (map[i] == EMPTY) {
					++sectorCount;
				}
			}
			printInt(sectorCount);
			if (sectorCount < *sectors) {
				printInt(777);
				*sectors = INSUFFICIENT_SECTORS; //sectors = -3
				return;
			} else {
				clear(files + fileIndex * DIR_ENTRY_LENGTH, DIR_ENTRY_LENGTH);
				files[fileIndex * DIR_ENTRY_LENGTH] = parentIndex;
				for (i = 0; i < MAX_FILENAME; ++i) {
					if (path[i] != '\0') {
						files[fileIndex * DIR_ENTRY_LENGTH + i +1] = path[i];
					} else {
						break;
					}
				}
				for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
					if (map[i] == EMPTY) {
						map[i] = USED;
						sectorz[fileIndex * DIR_ENTRY_LENGTH + sectorCount] = i;
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
			printInt(1010);
			*sectors = INSUFFICIENT_DIR_ENTRIES;
			return;
		}
		writeSector(map, MAP_SECTOR);
		writeSector(files, FILE_SECTOR);
		writeSector(sectorz, SECTORS);
		//*sectors = 0;
	}
}

void executeProgram(char *path, int segment, int *result, char parentIndex){
	
	// executeProgram baru #copas
	// char curdir;
	// char argc;
	// char *argv[2];
	// curdir = 0xFF; //root
	// argc = 2;
	// argv[0] = "abc";
	// argv[1] = "123";
	// interrupt(0x21, 0x20, curdir, argc, argv); 

	// executeProgram lama
	int i;
	char buffer[MAX_SECTORS * SECTOR_SIZE];
	//printString(path);
	readFile(buffer, path, result, parentIndex);
	
	if (*result == 0)
	{
		//printString("Success\0");
		for (i = 0; i < MAX_SECTORS * SECTOR_SIZE; i++)
		{
			putInMemory(segment, i, buffer[i]);
		}
		//printString(path);
		launchProgram(segment);
	}
	else //if (*result == FALSE)
	{
		printString("Failure\0");		
	}
}

void terminateProgram (int *result) {
	char shell[6];

	//interrupt(0x21,0xFF << 8 | 0x00,"Yesh\0",0,0);
	shell[0] = 's';
	shell[1] = 'h';
	shell[2] = 'e';
	shell[3] = 'l';
	shell[4] = 'l';
	shell[5] = '\0';
	executeProgram(shell, 0x2000, result, 0xFF);
}

void makeDirectory(char *path, int *result, char parentIndex){
	//Validasi path
	int i, j, idx, jdx, isFile;
	int dirIndex;
	int a,b,c,d;
	int check;
	char dirs[SECTOR_SIZE];
	readSector(dirs, DIR_SECTOR);
	// char files[SECTOR_SIZE];
	// readSector(files, FILE_SECTOR);
	//Cek path = direktori atau file
	j=0;
	while(path[j]!='\0' && path[j]!='/') {
		j++;
	}
	if (path[j]=='\0'){
		isFile=1;
		//printInt(111);
	}
	else if (path[j]=='/'){
		isFile=0;
		//printInt(777);
	}
	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
		d=0;
		check = 1;
		for (j = 0; path[j]!='\0' && path[j]!='/'; j++) {
			a=path[j];
			b=dirs[i+j+1];
			//c=files[i+j];
			jdx = j;
			// if (a != b && isFile == 0) {
			// 	check = 0;
			// 	break;
			// }
			// else if (a != b && isFile == 1) {
			// 	check = 0;
			// 	break;
			// }
			if (a != b) {
				if (d==1){
					//printInt(b);
					//interrupt(0x10, 0xE00 + path[jdx], 0, 0, 0);
				}
				check = 0;
				break;
			}
			else{
				//interrupt(0x10, 0xE00 + path[jdx], 0, 0, 0);
				d=1;
			}
		}
		if (check == 1 && isFile==0) {
			if(dirs[i]==parentIndex){
				idx=div(i,DIR_ENTRY_LENGTH);
				//printString(path+jdx+2);
				//printInt(idx);
				makeDirectory(path+jdx+2,result,idx);
				//printInt(*result);
				return;
			}
			else{
				//printString("oh no!");
				check = 0;
			}
		}
		else if (check == 1 && isFile==1){
			// harusnya dirs[i] ga sih? soalnya directory baru
			if(dirs[i]==parentIndex){
				idx=div(i,DIR_ENTRY_LENGTH);
				*result = -2;
				// result = -2
				return;
			}
		}
	}
	// validasi sektor/file
	if (check == 0 && isFile == 0) {
		*result = -1;
		//printString(path);
		return;
	} else {
		//Selesai validasi path

		for (dirIndex = 0; dirIndex < MAX_FILES; ++dirIndex) {
			if (dirs[dirIndex * DIR_ENTRY_LENGTH] == '\0' && dirs[dirIndex * DIR_ENTRY_LENGTH + 1] == '\0') {
				break;
			}
		}

		if (dirIndex < MAX_FILES) {
				clear(dirs + dirIndex * DIR_ENTRY_LENGTH, DIR_ENTRY_LENGTH);
				dirs[dirIndex * DIR_ENTRY_LENGTH] = parentIndex;
				for (i = 0; i < MAX_FILENAME; i++) {
					if (path[i] != '\0') {
						dirs[dirIndex * DIR_ENTRY_LENGTH + i +1] = path[i];
					} else {
						break;
					}
				}
				*result = 0;
		} else {
			*result = -3;
			return;
		}
		writeSector(dirs, DIR_SECTOR);
		return;
	}
}

void deleteFile(char *path, int *result, char parentIndex){
	int i, j, idx, jdx, z, isFile;
	int a,b,c;
	int check;
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	char map[SECTOR_SIZE];
	char sectors[SECTOR_SIZE];
	char buffer[SECTOR_SIZE];
	readSector(dirs, DIR_SECTOR);
	readSector(files, FILE_SECTOR);
	//Cek path = direktori atau file
	j=0;
	while(path[j]!='\0' && path[j]!='/') {
		j++;
	}
	if (path[j]=='\0'){
		isFile=1;
		// barusan isFile = 0
	}
	else if (path[j]=='/'){
		isFile=0;
		// barusan isFile = 1
	}
	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
		check = 1;
		for (j = 0; path[j] !='\0' && path[j]!='/'; j++) {
			a=path[j];
			b=dirs[i+j+1];
			c=files[i+j+1];
			jdx=j;
			if (a != b && isFile == 0) {
				check = 0;
				
				break;
			}
			else if (a != c && isFile == 1) {
				check = 0;
				
				break;
			}
		}
		if (check == 1 && isFile==0) {
			if(dirs[i]==parentIndex){
				idx=div(i,DIR_ENTRY_LENGTH);
				deleteFile(path+jdx+2,result,idx);
				// tadi parentIndex
				return;
			}
			else{
				check=0;
			}
		}
		else if (check == 1 && isFile==1){
			if(files[i]==parentIndex){
				idx=div(i,DIR_ENTRY_LENGTH);
				break;
			}
			else{
				check=0;
			}
		}
	}
	// validasi sektor/file
	if (check == 0) {
		*result = NOT_FOUND;
		return;
	} else {
		files[idx*DIR_ENTRY_LENGTH] = NUL;
		readSector(map, MAP_SECTOR);
		readSector(sectors, SECTORS);
		// Step 5: Baca daftar sektor file pada entri file
		// Step 6: Mark setiap byte pada sektor tersebut menjadi kosong
		for (i = 0; i < DIR_ENTRY_LENGTH; i++)
		{
			z = sectors[idx*DIR_ENTRY_LENGTH + i];
			clear(buffer,SECTOR_SIZE);
			writeSector(buffer,z);
			map[sectors[idx*DIR_ENTRY_LENGTH + i]] = EMPTY;
			sectors[idx*DIR_ENTRY_LENGTH + i] = EMPTY;
		}
		writeSector(map, MAP_SECTOR);
		writeSector(sectors, SECTORS);
		*result = 0;
		for (i = 0; i < DIR_ENTRY_LENGTH; i++){
			files[idx*DIR_ENTRY_LENGTH + i]=EMPTY;
		}
		writeSector(files, FILE_SECTOR);
	}
}

void deleteDirectory(char *path, int *success, char parentIndex)
{
	int i, j, idx, jdx, z, isFile;
	int a,b,c;
	int check;
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	char map[SECTOR_SIZE];
	char sectors[SECTOR_SIZE];
	clear(dirs,SECTOR_SIZE);
	clear(files,SECTOR_SIZE);
	clear(map,SECTOR_SIZE);
	clear(sectors,SECTOR_SIZE);
	readSector(dirs, DIR_SECTOR);
	readSector(files, FILE_SECTOR);
	//Cek path = direktori atau file
	j=0;
	//printInt(777);
	while(path[j]!='\0' && path[j]!='/') {
		j++;
	}
	if (path[j]=='\0'){
		isFile=1;
		// barusan isFile = 0
	}
	else if (path[j]=='/'){
		isFile=0;
		// barusan isFile = 1
	}
	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
		check = 1;
		for (j = 0; path[j] !='\0' && path[j]!='/'; j++) {
			a=path[j];
			b=dirs[i+j+1];
			// c=files[i+j];
			jdx=j;
			if (a != b) {
				check = 0;
				break;
			}
		}
		if (check == 1 && isFile==0) {
			if(dirs[i]==parentIndex && dirs[i+1]!='\0'){
				idx=div(i,DIR_ENTRY_LENGTH);
				deleteDirectory(path+jdx+2,success,idx);
				return;
			}
			else{
				check=0;
			}
		}
		else if (check == 1 && isFile==1){
			// Diganti di sini bro....
			// dari files[i]
			if(dirs[i]==parentIndex && dirs[i+1]!='\0'){
				idx=div(i,DIR_ENTRY_LENGTH);
				break;
			}
			else{
				check=0;
			}
		}
	}
	// validasi sektor/file
	if (check == 0) {
		*success = NOT_FOUND;
		return;
	} else {
		readSector(map, MAP_SECTOR);
		readSector(sectors, SECTORS);
		printInt(777);
		// Step 5
		/*for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
			if (files[i] == idx) { // parent file == idx (direktori ini)
				files[i] = NUL;
				for (j = 0; j < DIR_ENTRY_LENGTH; j++)
				{
					map[sectors[i + j]] = EMPTY;
				}
			}
		}*/
		// Step 6
		deleteDirectory2(map, sectors, dirs, files, idx);
		for (i = 0;i < DIR_ENTRY_LENGTH; i++){
			dirs[idx*DIR_ENTRY_LENGTH + i]=EMPTY;
		}
		*success = 0;
		writeSector(map, MAP_SECTOR);
		writeSector(sectors, SECTORS);
	}
	writeSector(dirs, DIR_SECTOR);
	writeSector(files, FILE_SECTOR);
}

void deleteDirectory2 (char map[SECTOR_SIZE], char sectors[SECTOR_SIZE], char dirs[SECTOR_SIZE], char files[SECTOR_SIZE], char parentIndex)
{
	int i, j, mark, idx;
	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
		if (dirs[i] == parentIndex && dirs[i+1]!='\0') {// parent file == idx (direktori ini)
			for(j=0;j<DIR_ENTRY_LENGTH;j++){
				dirs[i+j] = EMPTY;
			}
			//printInt(777);
			idx=div(i,DIR_ENTRY_LENGTH);
			deleteDirectory2(map, sectors, dirs, files, idx);
		}
		if (files[i] == parentIndex && files[i+1]!='\0') { // parent file == idx (direktori ini)
			for(j=0;j<DIR_ENTRY_LENGTH;j++){
				files[i+j] = EMPTY;
			}
			idx=div(i,DIR_ENTRY_LENGTH);
			for (j = 0; j < DIR_ENTRY_LENGTH; j++) {
				map[sectors[i + j]] = EMPTY;
				sectors[i + j] = EMPTY;
			}
		}
	}
}

void putArgs (char curdir, char argc, char argv[81][81]) {
	char args[SECTOR_SIZE];
	int i, j, p;
	clear(args, SECTOR_SIZE);
	      
	args[0] = curdir;    
	args[1] = argc;    
	i = 0;    
	j = 0;   
	for (p = 2; p < ARGS_SECTOR && i < argc; p++) {       
		args[p] = argv[i][j];
		if (argv[i][j] == '\0') {          
			i++;          
			j = 0;       
		} else {          
			j++;       
		}    
	}      
	writeSector(args, ARGS_SECTOR);  
}    

void getCurdir (char *curdir) {    
	char args[SECTOR_SIZE];    
	readSector(args, ARGS_SECTOR);    
	*curdir = args[0];  
}    

void getArgc (char *argc) {    
	char args[SECTOR_SIZE];    
	readSector(args, ARGS_SECTOR);    
	*argc = args[1];  
}    

void getArgv (char index, char *argv) {    
	char args[SECTOR_SIZE];    
	int i, j, p;    
	readSector(args, ARGS_SECTOR);
	
	i = 0;    
	j = 0;    
	for (p = 2; p < ARGS_SECTOR; p++) {       
		if (i == index) {          
			argv[j] = args[p];         
			++j;       
		}         
		
		if (args[p] == '\0') {          
			if (i == index) {            
				break;          
			} else {          
				++i;          
			}       
		}    
	}  
}  

void printInt(int x) {
	int a = 1;
	int b;
	if (x > 0) {
		
		while (div(x, a) != 0) {
			a *= 10;
		}
		a /= 10;
		while (a != 0) {
			b = div(x, a);
			interrupt(0x10, 0xE00 + (b + '0'), 0, 0, 0);
			x = x - b * a;
			a /= 10;
		}
	}
	else if(x == 0) {
		interrupt(0x10, 0xE00 + '0', 0, 0, 0);
	}
	else if(x < 0) {
		x *= -1;
		interrupt(0x10, 0xE00 + '-', 0, 0, 0);
		while (div(x, a) != 0) {
			a *= 10;
		}
		a /= 10;
		while (a != 0) {
			b = div(x, a);
			interrupt(0x10, 0xE00 + (b + '0'), 0, 0, 0);
			x = x - b * a;
			a /= 10;
		}
	}
	interrupt(0x10, 0xE*256+0xa, 0, 0, 0); 
	interrupt(0x10, 0xE*256+0xd, 0, 0, 0);	
}

/*
void validatePath(char *path, int *result, char parentIndex)
{
	//Validasi path
	int i, j, idx, jdx, isFile;
	int check;
	char dirs[SECTOR_SIZE];
	readSector(dirs, DIR_SECTOR);
	char files[SECTOR_SIZE];
	readSector(files, FILE_SECTOR);
	//Cek path = direktori atau file
	int j=0;
	while(path[j]!='\0' || path[j]!='/') {
		j++;
	}
	if (path[j]=='\0'){
		isFile=1;
	}
	else if (path[j]=='/'){
		isFile=0;
	}
	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
		check = 1;
		for (j = 1; j !='\0' && path[j]!='/'; j++) {
			int a,b,c;
			a=path[j];
			b=dirs[i+j];
			c=files[i+j];
			if (a != b && isFile == 0) {
				check = 0;
				break;
			}
			else if (a != c && isFile == 1) {
				check = 0;
				break;
			}
		}
		if (check == 1 && path[jdx]=='/') {
			if(dirs[i]==parentIndex){
				idx=i;
				validatePath(path+jdx+1,result,idx);
				break;
			}
		}
		else if (check == 1 && path[jdx]=='\0'){
			if(files[i]==parentIndex){
				idx=i;
				*result=ALREADY_EXISTS;
				// result = -2
				return;
			}
		}
	}
}
*/
