#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <stdint.h>


int main(int argc, const char* argv[])
{
	std::string filename = "Dummy.img";
	size_t fsize = 64*1024;
	unsigned char* buffer;
	if (argc >= 2)
	{
		filename = argv[1];
	}
	
	
	if (argc >= 3)
	{
		fsize = std::atoi(argv[2]);
	}
	
	
	buffer = new unsigned char[fsize];
	memset(buffer, 0, fsize);
	std::ofstream file(filename, std::ios::binary);
	
	size_t i = 0;
	while (i < fsize)
	{
		*reinterpret_cast<uint32_t*>(buffer) = 0xDEADBABA;
		i += 4;
	}
	
	
	
	
	
	
	std::copy(buffer, buffer + fsize, std::ostreambuf_iterator<char>(file));
	file.flush();
	file.close();
	std::cout << "Done." << std::endl;
}