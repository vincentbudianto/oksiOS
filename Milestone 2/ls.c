int main()
{
    char dirs[512];
    char res[15];
    int *result;
    int i;
    int j;
    char curdir;
    char argc;
    char argv[81][81];
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    for (i = 0; i < argc; ++i) {
        interrupt(0x21, 0x23, i, argv[i], 0);
    }

    interrupt(0x21,0xFF << 8 | 0x02,dirs, 0x101,0);
    for (i = 0; i < 512; i+=16) {
		if (curdir == dirs[i])
        {
            for (j = 1; dirs[i + j] != '\0'; j++)
            {
                res[j - 1] = dirs[i + j];
            }
            interrupt(0x21,0xFF << 8 | 0x00,res,0,0);
        }
	}
    interrupt(0x21,0xFF << 8 | 0x07, 0, 0, 0);

    return 0;
}