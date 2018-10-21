#ifndef INCLUDED_KERNEL_SCHEDULING_PROCESS_RESOURCES_H
#define INCLUDED_KERNEL_SCHEDULING_PROCESS_RESOURCES_H

#include <Common.h>

#ifdef __cplusplus
#include "process_resources.hh"
#endif

START_NS(Kernel)
C_CODE

typedef struct process_resources presources_t;

C_END
END_NS

#endif