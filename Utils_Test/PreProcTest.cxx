#define C_CODE(...) namespace __VA_ARGS__ { extern "C" {
#define C_END(...) __VA_OPT__(,) }

C_CODE(Kernel)


struct OpenFile_Hndl
{
	
};

C_END(Kernel)