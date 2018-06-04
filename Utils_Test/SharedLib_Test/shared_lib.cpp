
extern "C" {


int sl_z = 1;;

int sl_bar()
{
	if (sl_z > 4)
	{
		return 1;
	}
	
	return 0;
}

int sl_foo()
{
	if (sl_z)
	{
		return sl_bar();
	}
	
	return -4;
}

}