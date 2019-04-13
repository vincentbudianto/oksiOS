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
int equals(char*,char*);
int mod(int a, int b);
int div(int a, int b);
void printInt(int x);
void clearString(char *a, int n);

int *success, *parallel, segment;
char parentIndex, command[81], argv[81][81], args[81]; 
int argx;

int main()
{
 	int i,j,l;
	char k, *x, d[512], y[81], targetDirectory;

	enableInterrupts();
	while (1)
	{
		parentIndex = 0xFF;
		//interrupt(0x21, 0x21, &parentIndex, 0, 0);
		//interrupt(0x21, 0x02, d, 0x101, 0);
		//printInt(777);
		interrupt(0x21,0xFF << 8 | 0x00,"\r$ ",0,0);

		// if (parentIndex == 0xFF)
		// {
		interrupt(0x21,0xFF << 8 | 0x00,"root/ ",0,0);
		// }
		// else
		// {
		// 	interrupt(0x21,0xFF << 8 | 0x00,d + (parentIndex * 16 + 1),0,0);
		// 	interrupt(0x21,0xFF << 8 | 0x00,"/ ",0,0);
		// }
		
		interrupt(0x21,0xFF << 8 | 0x01,command,1,0);
		i = 0;
		j = 0;
		
		for (l = 0; l < 81; ++l)
		{
			clearString(argv, 81);
		}

		clearString(args, 81);
		// clearString(argc, 1);

		*parallel = 0;
		argx = 0;

		while(command[i] != '\0' && command[i] != ' ')
		{
			i++;
		}
		
		k = 0;
		
		if(command[i] == ' ')
		{
			command[i] = '\0';
			i++;
			argx++;
		
			while(command[i] != '\0')
			{
				// argv[k][j] = command[i];
			
				// if (argv[k][j] == ' ')
				// {
				// 	argv[k][j] = '\0';
				// 	k = k + 1;
				// 	j = 0;
				// }
				//else 
				if (argv[k][j] == '&')
				{
					*parallel = 1;
				}
				// else
				// {
				// 	j++;
				// }
				
				i++;
			}
		}
		// 	k = k + 1;
		// 	// interrupt(0x21,0x22,x,0,0);
		// 	// interrupt(0x21,0x23,0,y,0);
		// }
		// printInt(777);

		// interrupt(0x21, 0x20, parentIndex, k, argv);

		if (equals(command,"pause\0"))
		{
			// interrupt(0x21,0x22,argc,0,0);
		
			if (argx > 0)
			{
				interrupt(0x21,0x23,0,args,0);
				segment = (args[0]-'0'+2) << 12; //cuma bisa sampai 9 pid nya
				interrupt(0x21,0x32,segment,success);
			}
			else
			{
			}
		}

		if (equals(command,"resume\0"))
		{
			// interrupt(0x21,0x22,argc,0,0);

			if (argx>0)
			{
				interrupt(0x21,0x23,0,args,0);
				segment = (args[0]-'0'+2) << 12; //cuma bisa sampai 9 pid nya
				interrupt(0x21,0x33,segment,success);
				interrupt(0x21,0x31,0,0,0);
			}
			else
			{
			}
		}

		if (equals(command,"kill\0"))
		{
			// interrupt(0x21,0x22,argc,0,0);
		
			if (argx>0)
			{
				interrupt(0x21,0x23,0,args,0);
				segment = (args[0]-'0'+2) << 12; //cuma bisa sampai 9 pid nya
				interrupt(0x21,0x34,segment,success);
			}
			else
			{
			}
		}
		else
		{
			//interrupt(0x21,0x22,argc,0,0);
		
			if (command[0] != '\0')
			{
				if (command[1] != '\0')
				{
					if (command[0] == '.' && command[1] == '/')
					{	
						if (*parallel == 0)
						{
							interrupt(0x21,parentIndex << 8 | 0x06,command+2,success);
						}
						else
						{
							interrupt(0x21,parentIndex << 8 | 0x35,command+2,success);
						}
					}
					else
					{
						if (*parallel == 0)
						{
							interrupt(0x21,0xFF << 8 | 0x06,command,success);
						}
						else
						{
							printInt(777);
							interrupt(0x21,0xFF << 8 | 0x35,command,success);
							printInt(777);
						}
					}
				}
				else
				{
					if (*parallel == 0)
					{
							interrupt(0x21,0xFF << 8 | 0x06,command,success);
					}
					else
					{
							printInt(777);
							interrupt(0x21,0xFF << 8 | 0x35,command,success);
							printInt(777);
					}
				}
			}
			else
			{
				interrupt(0x21,0,"empty command\0",0,0);
				interrupt(0x10, 0xE*256+'\n', 0, 0, 0); 
				interrupt(0x10, 0xE*256+'\r', 0, 0, 0);			
			}
		}
	}

	return 0;
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

int mod(int a, int b)
{
	while(a >= b)
	{
		a = a - b;
	}

	return a;
}

int div(int a, int b)
{
	int q = 0;

	while (q*b <= a)
	{
		q = q+1;
	}

	return q-1;
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
}

void clearString(char *a, int n)
{
    int i;

    for (i = 0; i < n; ++i)
		{
        a[i] = '\0';
    }
}