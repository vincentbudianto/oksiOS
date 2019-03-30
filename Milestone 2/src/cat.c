// Implementasi belum benar 100%

int main()
{
    char res[15];
    char buffer[13312];
    int *result;
    int i;
    char curdir;
    char argc;
    char argv[81][81];
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    
    if (argc > 0)
    {
        // ++ penanganan tambahan kalau file tidak ada
        if (argc > 1)
        {
            if (argv[1][0] == '-' && argv[1][1] == 'w')
            {
                interrupt(0x21,0xFF << 8 | 0x01,buffer,0,0);
                interrupt(0x21,0xFF << 8 | 0x05,buffer,argv[0],result);
            } else {
                interrupt(0x21,0xFF << 8 | 0x04,buffer,argv[0],result);
                interrupt(0x21,0xFF << 8 | 0x00,buffer,0,0);
            }
        } else {
            interrupt(0x21,0xFF << 8 | 0x04,buffer,argv[0],result);
            interrupt(0x21,0xFF << 8 | 0x00,buffer,0,0);
        }
        interrupt(0x21,0xFF << 8 | 0x07, result, 0, 0);
    } else {
        interrupt(0x21,0xFF << 8 | 0x07, -1, 0, 0);
    }

    return 0;
}