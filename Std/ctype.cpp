#include "ctype.h"


C_CODE

int isdigit(int c)
{
	return c >= '0' && c <= '9';
}


C_END