#include "random.h"

#include <stdlib.h>
#include <time.h>

void rand_init(void)
{
	srand(time(NULL));
}

uint8_t rand_get(void)
{
	return (uint8_t) rand();
}

