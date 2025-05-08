#include <cstdio>

#include "Core.h"

void Testfunc()
{
#ifdef OGS_EXPORTS
    printf("Hello, World exported!\n");
#else
    printf("Hello, World!\n");
#endif
}
