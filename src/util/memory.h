#pragma once

typedef unsigned long long size_t;

void *mem_allocate(size_t size);
void mem_free(void *mem);