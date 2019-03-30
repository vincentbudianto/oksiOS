// Implementasi belum benar 100%

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
        interrupt(0x21,0xFF << 8 | 0x09,argv[0], result1,curdir);
        interrupt(0x21,0xFF << 8 | 0x0A,argv[0], result2,curdir);
        if (result1 == 0 || result2 == 0)
        {
            interrupt(0x21,0xFF << 8 | 0x07, 0, 0, 0);
        }
        else
        {
            interrupt(0x21,0xFF << 8 | 0x07, result1, 0, 0);
        }
    } else {
        interrupt(0x21,0xFF << 8 | 0x07, -1, 0, 0);
    }

    return 0;
}