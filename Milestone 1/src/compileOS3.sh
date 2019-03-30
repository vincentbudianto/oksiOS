#!/bin/bash
as86 lib.asm -o lib_asm.o
bcc -ansi -c -o kalku.o kalku.c
ld86 -o kalku -d kalku.o lib_asm.o
./loadFile kalku