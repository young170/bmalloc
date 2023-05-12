#include <stdio.h>
#include "bmalloc.h"

int 
main ()
{
	bmprint() ;

    printf("> Size of bm_header struct: %lu\n", sizeof(bm_header));
}
