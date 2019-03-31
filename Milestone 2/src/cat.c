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
                    interrupt(0x10, 0xE*256+'\n', 0, 0, 0); 
			        interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
                }
            } else {
                interrupt(0x21,curdir << 8 | 0x04,buffer,argv[0],&result);
                if (result < 0)
                {
                    interrupt(0x21,0xFF << 8 | 0x00,"No files read\0",0,0);
                    interrupt(0x10, 0xE*256+'\n', 0, 0, 0); 
			        interrupt(0x10, 0xE*256+'\r', 0, 0, 0);			
                } else {
                    interrupt(0x21,curdir << 8 | 0x00,buffer,0,0);
                    interrupt(0x10, 0xE*256+'\n', 0, 0, 0); 
                    interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
                }  
                
            }
        } else {
            interrupt(0x21,curdir << 8 | 0x04,buffer,argv[0],&result);
            if (result < 0)
            {
                interrupt(0x21,0xFF << 8 | 0x00,"No files read\0",0,0);
                interrupt(0x10, 0xE*256+'\n', 0, 0, 0);
                interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
            } else {
                interrupt(0x21,curdir << 8 | 0x00,buffer,0,0);
                interrupt(0x10, 0xE*256+'\n', 0, 0, 0);
                interrupt(0x10, 0xE*256+'\r', 0, 0, 0);
            }
                
        }
        interrupt(0x21,curdir << 8 | 0x07, &result, 0, 0);
    } else {
        interrupt(0x21,curdir << 8 | 0x07, -1, 0, 0);
    }

    return 0;
}