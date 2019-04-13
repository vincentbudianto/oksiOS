int main()
{
    int c, i, j;
    char x, curdir, argc, argv[81][81];

    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);

    for (x = 0; x < argc; ++x)
    {
        interrupt(0x21, 0x23, x, argv[x], 0);
	}

    interrupt(0x21, 0x40, 10, 0, 0);

    if (argc > 0)
    {    
        for (i = 0; i < argc; i++)
        {
            interrupt(0x21, 0x00, argv[i], 0, 0);
            interrupt(0x21, 0x00, " ", 0, 0, 0);
        }
    
        interrupt(0x10, 0xE*256+'\n', 0, 0, 0); 
		interrupt(0x10, 0xE*256+'\r', 0, 0, 0);			
        interrupt(0x21,curdir << 8 | 0x07,0, 0, 0);
    }
    else
    {
        interrupt(0x21,0xFF << 8 | 0x00,"No arguments\0",0,0);
        interrupt(0x10, 0xE*256+'\n', 0, 0, 0); 
		interrupt(0x10, 0xE*256+'\r', 0, 0, 0);			
        interrupt(0x21,curdir << 8 | 0x07,0,0,0);
    }
    
    return 0;
}