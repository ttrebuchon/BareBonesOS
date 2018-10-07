#ifndef INCLUDED_KERNEL_SHELL_RESULT_H
#define INCLUDED_KERNEL_SHELL_RESULT_H

#include <Common.h>

#ifdef __cplusplus
#include <Utils/EnumOperators.hh>
#endif

START_NS(Kernel)
C_CODE

typedef enum shell_result_state
{
	SHELL_RESULT_STATE_OK = 0,
	
	SHELL_RESULT_STATE_GOOD = 0x1000,
	SHELL_RESULT_STATE_DONE = 0x1001,
	
	
	SHELL_RESULT_STATE_FAILED = 0x2000,
	SHELL_RESULT_STATE_BAD = 0x2000,
	SHELL_RESULT_STATE_START_FAILED = 0x2001,
	SHELL_RESULT_STATE_EXEC_FAILED = 0x2002,
}
shell_result_state_t;

#ifdef __cplusplus
}

ENUM_OPS(shell_result_state, uint16_t);

extern "C"
{
#endif

typedef struct shell_result
{
	shell_result_state_t state;
	int exit_code;
	char* output;
}
shell_result_t;


int shell_result_is_good(const struct shell_result*);
int shell_result_is_bad(const struct shell_result*);



C_END
END_NS // Kernel

#endif