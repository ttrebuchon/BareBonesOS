#include <kernel/Utility.hh>

ASM_EXEC(Test, : "=r"(y) : "r"(z))

ASM_MOV(%SP, %0, y)

ASM_OP1(mov, %0)

ASM_OP2(mov, %SP, %0)