#include "result.h"


int shell_result_is_good(const struct shell_result* res)
{
	assert(res);
	if (((res->state & SHELL_RESULT_STATE_GOOD) != 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int shell_result_is_bad(const struct shell_result* res)
{
	assert(res);
	if (((res->state & SHELL_RESULT_STATE_BAD) != 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}