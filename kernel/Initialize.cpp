#include "Initialize.hh"


static bool stdlib_initialized_value = false;

void initialize_stdout();


void initialize_stdlib()
{
    initialize_stdout();

	
	stdlib_initialized_value = true;
}

bool stdlib_initialized()
{
	return stdlib_initialized_value;
}