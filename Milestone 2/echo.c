int mod(int a, int b);
int div(int a, int b);
void printInt(int x);

int main()
{
    int c;
    int i;
    int j;
    char x;
    char curdir;
    char argc;
    char argv[81][81];
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);

    for (x = 0; x < argc; ++x) {
        interrupt(0x21, 0x23, x, argv[x], 0);
	}

    interrupt(0x21,0xFF << 8 | 0x00,"CheckTime\0",0,0);
    interrupt(0x21,0xFF << 8 | 0x00,argv[0],0,0);   

    if (argc > 0)
    {
        for (i = 0; i < argc; i++)
        {
            c = argv[i][0];
            j = 0;
            while (c != '\0')
            {
                interrupt(0x10, 0xE00 + c, 0, 0, 0);
                j++;
                c = argv[i][j];
            }
            interrupt(0x10, 0xE00 + ' ', 0, 0, 0);
        }
        interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
	    interrupt(0x10, 0xE*256+0xd, 0, 0, 0);
        interrupt(0x21,0xFF << 8 | 0x07,0, 0, 0);
    }
    else
    {
        interrupt(0x21,0xFF << 8 | 0x00,"No arguments\0",0,0);
        interrupt(0x21,0xFF << 8 | 0x07,0,0,0);
    }
    return 0;
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