#ifndef INCLUDED_STD_H
#define INCLUDED_STD_H

#include <Common.h>
#include <kernel/Memory.h>
#include <kernel/Filesystem/API.hh>
#include <kernel/Scheduling/API.h>

#include <kernel/Timer.h>



C_CODE

#ifdef __cplusplus
[[ noreturn ]]
#endif
void abort();

C_END
#endif