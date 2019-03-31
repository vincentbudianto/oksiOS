// Implementasi belum benar 100%
int mod(int a, int b);
int div(int a, int b);
void printInt(int x);


int main()
{
    char res[15];
    int *result1;
    int *result2;
    int i;
    char curdir;
    char argc;
    char argv[81][81];
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);

    for (i = 0; i < argc; ++i) {
        interrupt(0x21, 0x23, i, argv[i], 0);
    }
    
    if (argc > 0)
    {
        // Path belum relatif terhadap curdir
        interrupt(0x21,curdir << 8 | 0x09,argv[0], result1,curdir);
        printInt(*result1);
        interrupt(0x21,curdir << 8 | 0x0A,argv[0], result2,curdir);
        printInt(*result2);
        if (result1 == 0 || result2 == 0)
        {
            interrupt(0x21,curdir << 8 | 0x07, 0, 0, 0);
        }
        else
        {
            interrupt(0x21,curdir << 8 | 0x07, result1, 0, 0);
        }
    }
    else
    {
        interrupt(0x21,curdir << 8 | 0x07, -1, 0, 0);
    }

    return 0;
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