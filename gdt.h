#pragma once
#include <stdint.h>
#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint64_t gdtEntry;
extern void setGdt(gdtEntry* gdt, size_t gdtSize);
extern void reloadSegments();