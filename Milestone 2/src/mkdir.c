int main()
{
    int *result;
    int i;
    char curdir;
    char argc;
    char argv[81][81];
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    
    for (i = 0; i < argc; ++i) {
        interrupt(0x21, 0x23, i, argv[i], 0);
    }

    if (argc == 1)
    {
        interrupt(0x21,curdir << 8 | 0x08, argv[0], result ,curdir);
        if (*result!=0){
            interrupt(0x21,0,"Fail",0,0);
            interrupt(0x10, 0xE*256+'\n', 0, 0, 0);
            interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
        }
        interrupt(0x21,curdir << 8 | 0x07, result, 0, 0);
    }
    else
    {
        interrupt(0x21, curdir << 8 | 0x00, "No arguments", 0, 0);
        interrupt(0x10, 0xE*256+'\n', 0, 0, 0);
        interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
        interrupt(0x21,curdir << 8 | 0x07,-1, 0, 0);
    }

    return 0;
}