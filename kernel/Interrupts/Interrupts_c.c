#include "Interrupts.h"

extern void doCli();
extern void doSti();


void cli()
{
    doCli();
}

void sti()
{
    doSti();
}