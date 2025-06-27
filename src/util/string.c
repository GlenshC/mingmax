#include <stdio.h>
#include "util/memory.h"
#include "util/string.h"

char* string_read_file(const char* path)
{
    FILE *file = fopen(path, "r");

    fseek(file, 0, SEEK_END);
    unsigned long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *str = mem_allocate(size + 1);
    str[fread(str, sizeof(char), size, file)] = '\0';

    return str;
}