#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <libgen.h>


int main(int argc, const char* argv[])
{
	
	char dir_name[1024];
	char wd[1024];
	memset(wd, 0, 1024);
	strcpy(dir_name, argv[0]);
	strcpy(wd, dirname(dir_name));

	std::string lib_path;// = wd;
	


	std::string filename = "Dummy.img";
	size_t fsize = 512;
	size_t libsize;
	unsigned char* buffer;
	if (argc >= 2)
	{
		filename = argv[1];
	}

	if (argc >= 3)
	{
		//lib_path += "/";
		lib_path += argv[2];
	}
	else
	{
		lib_path += "libDynamic.so";
	}
	
	
	if (argc >= 4)
	{
		fsize = std::atoi(argv[3]);
	}

	FILE* lib = fopen(lib_path.c_str(), "r");
	if (!lib)
	{
		std::cout << strerror(errno) << std::endl;
	}
	assert(lib);

	fseek(lib, 0, SEEK_END);
	libsize = ftell(lib);
	fseek(lib, 0, SEEK_SET);

	size_t padding = 0;
	if ((libsize + 1) % 512 != 0)
	{
		padding = 512 - ((libsize + 1) % 512);
	}

	const size_t total_size = fsize + libsize + 1 + padding;
	assert(total_size % 512 == 0);
	
	
	buffer = new unsigned char[total_size];
	memset(buffer, 0, total_size);
	std::ofstream file(filename, std::ios::binary);
	
	size_t i = 0;
	while (i < fsize)
	{
		// *reinterpret_cast<uint32_t*>(buffer + i) = 0xDEADBABA;
		*reinterpret_cast<uint32_t*>(buffer + i) = i;
		i += 4;
	}

	*reinterpret_cast<uint64_t*>(buffer + fsize) = libsize;
	unsigned char* lib_buf = buffer + fsize + sizeof(uint64_t);
	fread(lib_buf, 1, libsize, lib);
	fclose(lib);
	
	
	std::cout << "Lib size: " << libsize << std::endl;
	
	
	
	std::copy(buffer, buffer + total_size, std::ostreambuf_iterator<char>(file));
	file.flush();
	file.close();
	std::cout << "Done." << std::endl;
}