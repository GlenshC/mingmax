#include <stdlib.h>
#include "util/memory.h"

void *mem_allocate(size_t size)
{
    return malloc(size);
}

void mem_free(void *mem)
{
    free(mem);
}