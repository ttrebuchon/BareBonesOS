
int __main();


int main()
{
	return __main();
}


#ifdef __NOSTDLIB__


void _start()
{
	main();
	//__builtin_exit(0);
	while (1);
}
#endif