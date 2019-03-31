// Implementasi belum benar 100%
void printInt(int x);
int mod(int a, int b);
int div(int a, int b);

int main()
{
    char res[15];
    char buffer[13312];
    int result;
    int i;
    char x;
    char curdir;
    char argc;
    char argv[81][81];
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    for (x = 0; x < argc; ++x) {
        interrupt(0x21, 0x23, x, argv[x], 0);
    }
    
    if (argc > 0)
    {
        // ++ penanganan tambahan kalau file tidak ada
        if (argc > 1)
        {
            if (argv[1][0] == '-' && argv[1][1] == 'w')
            {
                interrupt(0x21,curdir << 8 | 0x01,buffer,0,0);
                i = 0;
                result = 0;
                while (buffer[i] != '\0')
                {
                    i++;
                }
                i++;
                while (i > 0)
                {
                    result++;
                    i -= 512;
                }
                interrupt(0x21,curdir << 8 | 0x05,buffer,argv[0],&result);
                if (result < 0)
                {
                    interrupt(0x21,0xFF << 8 | 0x00,"Files can't be written\0",0,0);
                }
            } else {
                interrupt(0x21,curdir << 8 | 0x04,buffer,argv[0],&result);
                if (result < 0)
                {
                    interrupt(0x21,0xFF << 8 | 0x00,"No files read\0",0,0);
                } else {
                    interrupt(0x21,curdir << 8 | 0x00,buffer,0,0);
                }
                
            }
        } else {
            interrupt(0x21,curdir << 8 | 0x04,buffer,argv[0],&result);
            if (result < 0)
            {
                interrupt(0x21,0xFF << 8 | 0x00,"No files read\0",0,0);
            } else {
                interrupt(0x21,curdir << 8 | 0x00,buffer,0,0);
            }
                
        }
        interrupt(0x21,curdir << 8 | 0x07, &result, 0, 0);
    } else {
        interrupt(0x21,curdir << 8 | 0x07, -1, 0, 0);
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

void clearString(char *a, int n){
    int i;

    for (i = 0; i < n; ++i){
        a[i] = '\0';
    }
}