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
#define INSUFFICIENT_SECTORS 0
#define SUCCESS 0
#define NOT_FOUND -1
#define ALREADY_EXISTS -2
#define INSUFFICIENT_FILE_ENTRIES -3
#define INSUFFICIENT_DIR_ENTRIES -3
#define EMPTY 0x00
#define USED 0xFF
#define NUL '\0'
#define ARGS_SECTOR 512

void printString(char *string);
void cd(char *arg, char curdir);
int equals(char*,char*);
void printInt(int x);
int mod(int a, int b);
int div(int a, int b);
void clearString(char *a, int n);

int *success;
char parentIndex;
char command[81];
char argv[81][81];
char args[81];
char argc[1];

int main() {
 	int i,j,l;
	char k;
	char *x;
	char y[81];
	char targetDirectory;
	//interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
	parentIndex = 0xFF;

	//prompt
	interrupt(0x21,0xFF << 8 | 0x00,"\r$ ",0,0);

	//reading a command
	interrupt(0x21,0xFF << 8 | 0x01,command,0,0);

	i = 0;
	j = 0;
	
	for (l = 0; l < 81; ++l){
		clearString(argv, 81);
	}

	clearString(args, 81);
	clearString(argc, 1);

	while(command[i] != '\0' && command[i] != ' ') {
		i++;
	}
	if(command[i]==' '){
		command[i] = '\0';
		i++;
		k=0;
		while(command[i] != '\0'){
			argv[k][j]=command[i];
			if (argv[k][j]==' '){
				argv[k][j]='\0';
				k=k+1;
				j=0;
			}
			else{
				j++;
			}
			i++;
		};
		k=k+1;
		printInt(k);
		
		//interrupt(0x10, 0xE00 + argv[0][0], 0, 0, 0);
		printString(argv[0]);
		interrupt(0x21, 0x20, parentIndex, k, argv);
		interrupt(0x21,0x22,x,0,0);
		interrupt(0x21,0x23,0,y,0);
		printInt(*x);
		printString(y);
	}

	if(equals(command,"cd\0")) {
		interrupt(0x21,0x22,argc,0,0);
		if (argc[0]>0){
			interrupt(0x21,0x23,1,args,0);
			interrupt(0x21,0x21,parentIndex,0,0);
			printString(args);
			cd(args,parentIndex);
		}
		else{
			parentIndex=0xFF;
			interrupt(0x21, 0x20, parentIndex, k, argv);
			printString("Moved to root\0");
		}
	}

	else {
		printString(command);
		printString(argv[0]);
		interrupt(0x21,0x22,argc,0,0);
		printInt(argc[0]);
		if (command[0] != '\0')
		{
			if (command[1] != '\0')
			{
				if (command[0] == '.' && command[1] == '/')
				{	
					interrupt(0x21,0x21,targetDirectory,0,0);
					interrupt(0x21, 0x20, targetDirectory, k, argv);
					interrupt(0x21,0xFF << 8 | 0x06,command,0x2000,success);
				} else {
					targetDirectory = 0xFF;
					if (targetDirectory == 0xFF && k == 1)
					{
						interrupt(0x21,0xFF << 8 | 0x00,"Yesh\0",0,0);
						printString(argv[0]);
					}
					interrupt(0x21, 0x20, targetDirectory, k, argv);
					interrupt(0x21,0xFF << 8 | 0x06,command,0x2000,success);
				}
			} else {
				targetDirectory = 0xFF;
				interrupt(0x21, 0x20, targetDirectory, k, argv);
				interrupt(0x21,0xFF << 8 | 0x06,command,0x2000,success);
			}
		}
		else {
			interrupt(0x21,0,"empty command\0",0,0);
		}
	}
  interrupt(0x21,0xFF << 8 | 0x07,0, 0, 0);
	return 0;
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

int equals(char* str1 , char* str2)
{
	int i = 0;
	while(1)
	{
		if(str1[i] != str2[i])
			return 0;
		if(str1[i++] == '\0')
			return 1;
	}
}

void cd(char *arg, char curdir){
	int *result;
	int i, j, idx, jdx, isFile;
	int a,b,c;
	int check;
	char dirs[SECTOR_SIZE];
	interrupt(0x21,0x02,dirs, 0x101,0);

	

	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
		check = 1;
		for (j = 1; dirs[i+j] !='\0'; j++) {
			a=arg[j];
			b=dirs[i+j];
			jdx = j;
			if (a != b) {
				check = 0;
				printString("Not Found");
				break;
			}
			if (check == 1 && arg[jdx]=='/') {
				if(dirs[i]==curdir){
					idx=i/DIR_ENTRY_LENGTH;
					cd(arg+jdx+1,idx);
					break;
				}
			}
			else if (check == 1 && arg[jdx]=='\0'){
				// harusnya dirs[i] ga sih? soalnya directory baru
				if(dirs[i]==curdir){
					idx=i/DIR_ENTRY_LENGTH;
					parentIndex=idx;
					interrupt(0x21, 0x20, parentIndex, 0, argv);
					// result = -2
					return;
				}
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

void clearString(char *a, int n){
    int i;

    for (i = 0; i < n; ++i){
        a[i] = '\0';
    }
}