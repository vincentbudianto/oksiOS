int main()
{
    char dirs[512];
    char files[512];
    char res[15];
    int *result;
    int i,j,k;
    char x;
    char curdir;
    char argc;
    char argv[81][81];

    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);

    for (x = 0; x < argc; ++x) {
        interrupt(0x21, 0x23, x, argv[x], 0);
    }

    interrupt(0x21,curdir << 8 | 0x02,dirs, 0x101,0);
    for (i = 0; i < 512; i+=16) {
		if (curdir == dirs[i] && dirs[i + 1] != 0x00)
        {
            for (j = 1; dirs[i + j] != '\0'; j++)
            {
                res[j - 1] = dirs[i + j];
            }
            interrupt(0x21,curdir << 8 | 0x00,res,0,0);
            interrupt(0x10, 0xE*256+'\n', 0, 0, 0);
            interrupt(0x10, 0xE*256+'\r', 0, 0, 0);

            for (k = 0; k < 15; k++)
            {
                res[k] = 0;
            }
        }
	}

    interrupt(0x21,curdir << 8 | 0x02,files, 0x102,0);
    for (i = 0; i < 512; i+=16) {
		if (curdir == files[i]   && files[i + 1] != 0x00)
        {
            for (j = 1; files[i + j] != '\0'; j++)
            {
                res[j - 1] = files[i + j];
            }
            interrupt(0x21,curdir << 8 | 0x00,res,0,0);
            interrupt(0x10, 0xE*256+'\n', 0, 0, 0);
            interrupt(0x10, 0xE*256+'\r', 0, 0, 0);            
            for (k = 0; k < 15; k++)
            {
                res[k] = 0;
            }
        }
	}
    interrupt(0x21,curdir << 8 | 0x07, 0, 0, 0);

    return 0;
}