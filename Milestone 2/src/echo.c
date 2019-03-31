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
        interrupt(0x21,curdir << 8 | 0x07,0, 0, 0);
    }
    else
    {
        interrupt(0x21,0xFF << 8 | 0x00,"No arguments\0",0,0);
        interrupt(0x21,curdir << 8 | 0x07,0,0,0);
    }
    return 0;
}