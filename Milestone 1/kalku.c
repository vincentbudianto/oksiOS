void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int mod(int a, int b);
int div(int a, int b);
int mapInt(char x);
int operation(int a, char b, int c);
void printInt(int a);

char lineOpr[100];
char number[10];
char opr;
int a, b;

int main()
{
	int i;
	int x, y;
	int mult;

    printString("Masukkan operasi!");
	readString(lineOpr);

	mult = 1;
	x = 0;
	i = 0;
	while (lineOpr[i] == ' ') {
		i++;
	}
	if (lineOpr[i] == '-') {
		mult = -1;
		i++;
	}
	while (lineOpr[i] >= '0' && lineOpr[i] <= '9') {
		x = x * 10 + mapInt(lineOpr[i]);
		i++;
	}
	a = mult * x;

	while (lineOpr[i] == ' ') {
		i++;
	}
	opr = lineOpr[i];
	i++;
	while (lineOpr[i] == ' ') {
		i++;
	}

	y = 0;
	mult = 1;
	if (lineOpr[i] == '-') {
		mult = -1;
		i++;
	}
	while (lineOpr[i] >= '0' && lineOpr[i] <= '9') {
		y = y * 10 + mapInt(lineOpr[i]);
		i++;
	}
	b = mult * y;

	x = operation(a, opr, b);

	interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
	interrupt(0x10, 0xE*256+0xd, 0, 0, 0);
	printInt(a);
	interrupt(0x10, 0xE00 + ' ', 0, 0, 0);
	interrupt(0x10, 0xE00 + opr, 0, 0, 0);
	interrupt(0x10, 0xE00 + ' ', 0, 0, 0);
	printInt(b);
	interrupt(0x10, 0xE00 + ' ', 0, 0, 0);
	interrupt(0x10, 0xE00 + '=', 0, 0, 0);
	interrupt(0x10, 0xE00 + ' ', 0, 0, 0);
	printInt(x);
	interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
	interrupt(0x10, 0xE*256+0xd, 0, 0, 0);

	return 0;
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
	switch (AX) {
		case 0x0:
			printString(BX);
			break;
		case 0x1:
			readString(BX);
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

int mapInt(char x) {
	return (x - '0');
}

int operation(int a, char b, int c) {
	if (b == '+') {
		return (a + c);
	}
	else if (b == '-') {
		return (a - c);
	}
	else if (b == '*') {
		return (a * c);
	}
	else if (b == '/') {
		return (div(a, c));
	}
	else if (b == '%') {
		return (mod(a, c));
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
}