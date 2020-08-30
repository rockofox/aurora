#include "io.h"

void sleep(unsigned int millis)
{
    int ticksElapsed = 0;
    while (ticksElapsed / 57 < millis)
    {
        if (inb(0x40) == 0)
        {
            ticksElapsed++;
        }
    }
}
