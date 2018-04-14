#include <stdio.h>

__attribute__ ((constructor))
void foo()
{
	setvbuf(stdout, (char*)NULL, _IONBF, 0);
}
