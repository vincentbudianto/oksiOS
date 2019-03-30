#!/bin/bash
as86 logo.asm -o logo_asm.o
bcc -ansi -c -o logo.o logo.c
ld86 -o logo -d logo.o logo_asm.o
./loadFile logo
